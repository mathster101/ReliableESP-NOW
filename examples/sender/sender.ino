#include <ReliableEspNow.h>

void test1()
{
    reliableConnection rc;
    rc.init();
    //esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_LR);
    //char data[] = "abcde";
    char data[] = "Not only is the Beagle an excellent hunting dog and loyal companion, it is also happy-go-lucky, funny, and thanks to its pleading expression cute. They were bred to hunt in packs, so they enjoy company and are generally easygoing. There are two Beagle varieties: those standing under 13 inches at the shoulder, and those between 13 and 15 inches. Both varieties are sturdy, solid, and 'big for their inches,' as dog folks say. They come in such pleasing colors as lemon, red and white, and tricolor. The Beagle's fortune is in his adorable face, with its big brown or hazel eyes set off by long, houndy ears set low on a broad head. A breed described as 'merry' by its fanciers, Beagles are loving and lovable, happy, and companionable'all qualities that make them excellent family dogs. No wonder that for years the Beagle has been the most popular hound dog among American pet owners. These are curious, clever, and energetic hounds who require plenty of playtime.";
    while (true)
    {

        unsigned long start = millis();
        rc.sendData(data, strlen(data));
         unsigned long end = millis();

        //Serial.println((end - start));
        Serial.println("sent");
        delay(1000);
    }
}

void setup()
{
    delay(1000);
    Serial.begin(921600);
}

void loop()
{
    test1();
}
