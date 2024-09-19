#include <ReliableEspNow.h>

void test1()
{
    reliableConnection rc;
    rc.init();
    //esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_LR);
    byte *data;
    long counter = 0;
    while (true)
    {

        unsigned long start = millis();
        float size = rc.receiveData(&data);
        unsigned long end = millis();
        for (int c = 0; c < size; c++)
            Serial.print((char)*(data + c));
        Serial.print("\nData Rate = ");
        Serial.print( size / (end - start));
        Serial.println("KBps");
        ++counter;
        delete data;
        //delay(500);
    }
}

void setup()
{
    Serial.begin(921600);
}

void loop()
{
    test1();
}
