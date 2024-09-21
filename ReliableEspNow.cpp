#include "ReliableEspNow.h"

bool reliableConnection::init()
{
    esp.init();
    return true;
}

int reliableConnection::numPackets(int dataSize)
{
    if (dataSize % PDATA_SIZE != 0)
        return dataSize / PDATA_SIZE + 1;
    return dataSize / PDATA_SIZE;
}

// splits input data into packet friendly chunks
// sizeOfLastChunk = number of bytes to take from last packet(only)
std::vector<std::unique_ptr<byte>> reliableConnection::splitIntoChunks(const void *data, int size, int &sizeOfLastChunk)
{

    std::vector<std::unique_ptr<byte>> chunks;
    int numChunks = numPackets(size);
    // allocating the memory required
    for (int chunkNum = 0; chunkNum < numChunks; chunkNum++)
    {
        std::unique_ptr<byte> temp(new byte[PDATA_SIZE]);
        chunks.push_back(std::move(temp));
    }

    byte *now = (byte *)data;
    for (int chunkNum = 0; chunkNum < numChunks; chunkNum++)
    {
        // last packet
        if ((size - (now - (byte *)data)) < PDATA_SIZE)
        {
            sizeOfLastChunk = size - (now - (byte *)data);
            memcpy(chunks[chunkNum].get(), now, sizeOfLastChunk);
        }
        else
        {
            memcpy(chunks[chunkNum].get(), now, PDATA_SIZE);
        }

        now += PDATA_SIZE;
    }

    return chunks;
}

// split tcp packet into smaller radio packets
std::vector<dataPacket> reliableConnection::generatePackets(const void *data, int size)
{
    int sizeOfLastChunk = PDATA_SIZE;
    int numPack = numPackets(size);
    std::vector<std::unique_ptr<byte>> chunks = splitIntoChunks(data, size, sizeOfLastChunk);
    std::vector<dataPacket> packets;
    for (int pack = 0; pack < numPack; pack++)
    {
        dataPacket newPacket;
        newPacket.packetId = pack;
        if (pack == numPack - 1)
            newPacket.bytesToTake = sizeOfLastChunk;
        else
            newPacket.bytesToTake = PDATA_SIZE;
        memcpy(newPacket.rawData, chunks[pack].get(), newPacket.bytesToTake);
        packets.push_back(newPacket);
    }
    return packets;
}

// reassemble radio packets into tcp packet
byte *reliableConnection::reassembleChunks(std::vector<dataPacket> rcvdPackets, int &totalNumberofBytes)
{
    totalNumberofBytes = 0;
    for (int packetCount = 0; packetCount < rcvdPackets.size(); packetCount++)
    {
        totalNumberofBytes += rcvdPackets[packetCount].bytesToTake;
    }

    byte *merged = new byte[totalNumberofBytes];
    for (int packetCount = 0; packetCount < rcvdPackets.size(); packetCount++)
    {
        memcpy(merged + packetCount * PDATA_SIZE, rcvdPackets[packetCount].rawData, rcvdPackets[packetCount].bytesToTake);
    }
    return merged;
}

// reliably send one packet at a time
int reliableConnection::sendData(void *data, int size)
{
    int numberOfPackets = numPackets(size);
    int failureCounter = 0;
    if (numberOfPackets >= 255)
    {
        return -1;
    }

    std::vector<dataPacket> packets = generatePackets(data, size);
    ackPacket *ack;
    int numBytes;

    for (int i = 0; i < numberOfPackets;)
    {
        // Serial.print("sending packet = ");
        // Serial.println(i);
        dataPacket currentPacket = packets[i];
        esp.send(&currentPacket, sizeof(dataPacket));

        ack = (ackPacket *)esp.receive(numBytes, TIMEOUT);
        switch (numBytes)
        {
        case -1:
        case 0: // timeout so resend the same packet
        {
            // printf("<timeout> no ack\n");
            // Serial.println("timeout no ack");
            ++failureCounter;
            if (failureCounter > 5)
            {
                // printf("Total timeout...exiting\n");
                return -1;
            }
            break;
        }
        default:
        {
            failureCounter = 0;
            // printf("got an ack  #%d\n", ack.highestContinuous);
            // Serial.print("got an ack");
            // Serial.print(ack->highestContinuous);
            // Serial.print("  i=");
            // Serial.println(i);
            if (ack->highestContinuous == i)
                i++;
        }
        }
    }

    dataPacket finPacket;
    finPacket.packetId = numberOfPackets;
    finPacket.bytesToTake = -1 * numberOfPackets;
    memcpy(finPacket.rawData, "<END OF TRANSMISSION>\0", 23);

    for (int i = 0; i < 5; i++) // hail mary
        esp.send((void *)&finPacket, sizeof(dataPacket));

    return 0;
}

// reliably receive one packet at a time
int reliableConnection::receiveData(byte **data)
{

    int latestPacketId = 0;
    std::vector<dataPacket> rcvdPackets;
    int failureCounter = 0;

    while (true)
    {
        dataPacket *packet;
        ackPacket ack;
        int numBytes;
        packet = (dataPacket *)esp.receive(numBytes, TIMEOUT);
        // Serial.println("inside this loop thing");
        // Serial.println(numBytes);
        switch (numBytes)
        {
        case -1:
        case 0:
        {
            // Serial.println("data packet timeout");
            failureCounter += 1;
            if (failureCounter > 10 && latestPacketId >= 0)
            {
                // Serial.println("total data packet timeout");
                // printf("Total timeout...exiting\n");
                *data = nullptr;
                return -1;
            }
            break;
        }
        default:
        {
            if (packet->packetId == latestPacketId)
            {
                failureCounter = 0;
                if (packet->bytesToTake == -1 * latestPacketId) // fin packet
                {
                    int totalPacketSize;
                    *data = reassembleChunks(rcvdPackets, totalPacketSize);
                    return totalPacketSize;
                }
                // printf("got new packet %d\n", latestPacketId);
                // Serial.print("got data packet with ID=");
                // Serial.println(latestPacketId);
                ++latestPacketId;
                rcvdPackets.push_back(*packet);
            }

            // this is a repeated packet
            else
            {
                // Serial.println("this is a repeated packet");
                failureCounter += 1;
                if (failureCounter > 5) // quit if many retries
                {
                    *data = nullptr;
                    return -1;
                }
            }
            // resend ack if nothing new, else send ack with new val
            ack.highestContinuous = latestPacketId - 1;
            if (ack.highestContinuous > 200)
                continue;
            // Serial.print("sending ack ");
            // Serial.println(ack.highestContinuous);
            // printf("sending ack %d\n", ack.highestContinuous);

            esp.send((void *)&ack, sizeof(ackPacket));
        }
        }
    }
    return 0;
}

bool reliableConnection::isDataAvailable()
{
    // need to write fn to expose buffer size in sendNow
    return false;
}
