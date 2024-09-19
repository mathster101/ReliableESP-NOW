#include "SendNow.h"

void SenderBasic::init()
{
    esp_now_peer_info_t peerInfo;
    WiFi.mode(WIFI_STA);
    readyToSend = true;
    esp_now_init();
    memset(&peerInfo, 0, sizeof(peerInfo));

    // Register peer
    memcpy(peerInfo.peer_addr, targetMAC, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    esp_now_add_peer(&peerInfo);

    // Register Callbacks
    esp_now_register_send_cb(onDataSent);
    esp_now_register_recv_cb((esp_now_recv_cb_t)onDataRecv);

    esp_wifi_config_espnow_rate(WIFI_IF_STA, WIFI_PHY_RATE_MCS2_LGI);
    esp_wifi_set_bandwidth(WIFI_IF_STA, WIFI_BW_HT40);

    return;
}

void SenderBasic::send(void *data, int len)
{
    esp_err_t status = esp_now_send(targetMAC, (uint8_t *)data, len);
    //  Serial.println(status == ESP_OK ? "Delivery Success" : "Delivery Fail");
    // delay(30);
    return;
}

void *SenderBasic::receive(int &len, int timeout)
{
    float start = millis();
    while (true)
    {
        if (timeout != -1 && (millis() - start > timeout))
        {
            len = -1;
            return nullptr;
        }

        if (recvBuffer.size() > 0)
        {
            void *data = recvBuffer[0].data;
            len = recvBuffer[0].len;
            recvBuffer.erase(recvBuffer.begin());
            return data;
        }

        // delay(50);
    }
}

void SenderBasic::onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    UnitPacket newArrival;
    newArrival.len = len;
    newArrival.data = (void *)incomingData;
    if (recvBuffer.size() < MAXBUFFERSIZE)
    {
        recvBuffer.push_back(newArrival);
    }
    return;
}

void SenderBasic::changeTarget(uint8_t *newMac)
{
    memcpy(targetMAC, newMac, 6);

    // Register peer
    memcpy(peerInfo.peer_addr, targetMAC, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
        Serial.println("Failed to add peer");
    return;
}

//-----------------------------------------------------------------------------------------------------
