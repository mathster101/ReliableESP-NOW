#include <esp_now.h>
#include <SendNow.h>
#include <memory>
#include <cstring>

#define PDATA_SIZE 150
#define TIMEOUT 50

typedef uint8_t byte;

class dataPacket
{
public:
    int16_t packetId;
    int16_t bytesToTake;
    byte rawData[PDATA_SIZE];

    dataPacket()
    {
        packetId = 0;
        memset(rawData, 0, sizeof(char) * PDATA_SIZE);
    }
};

class ackPacket
{
public:
    byte highestContinuous;
    ackPacket() { highestContinuous = 0; }
    ackPacket(byte num) { highestContinuous = num; }
};

class reliableConnection
{
public:
    bool init();
    int sendData(void *data, int size);
    int receiveData(byte **data);
    bool isDataAvailable();
    SenderBasic esp;

private:
    int numPackets(int dataSize);
    std::vector<std::unique_ptr<byte>> splitIntoChunks(const void *data, int size, int &sizeOfLastChunk);
    std::vector<dataPacket> generatePackets(const void *data, int size);
    byte *reassembleChunks(std::vector<dataPacket> rcvdPackets, int &totalNumberofBytes);
};