#include <esp_now.h>
#include <WiFi.h>
#include <vector>
#include <cstdlib>
#include <esp_wifi.h>

#define MAXBUFFERSIZE 1

struct UnitPacket
{
    int len;
    void *data;
};

class SenderBasic
{
private:
    uint8_t targetMAC[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    inline static std::vector<UnitPacket> recvBuffer;
    esp_now_peer_info_t peerInfo;
    inline static bool readyToSend; // unused for now
    void static onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);
    void static onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status){};

public:
    void init();
    void send(void *data, int len);
    void *receive(int &len, int timeout = -1);
    void changeTarget(uint8_t *newMac);
};
//------------------------------------------------------------------------------------------------
