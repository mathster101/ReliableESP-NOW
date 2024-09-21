#include <ReliableEspNow.h>

void setup() {
  delay(1000);
  Serial.begin(921600);
}

void loop() {
  reliableConnection rc;
  rc.init();
  float rate = 0;
  char data2[] = "Not only is the Beagle an excellent hunting dog and loyal companion, it is also happy-go-lucky, funny, and thanks to its pleading expression cute. They were bred to hunt in packs, so they enjoy company and are generally easygoing. There are two Beagle varieties: those standing under 13 inches at the shoulder, and those between 13 and 15 inches. Both varieties are sturdy, solid, and 'big for their inches,' as dog folks say. They come in such pleasing colors as lemon, red and white, and tricolor. The Beagle's fortune is in his adorable face, with its big brown or hazel eyes set off by long, houndy ears set low on a broad head. A breed described as 'merry' by its fanciers, Beagles are loving and lovable, happy, and companionable'all qualities that make them excellent family dogs. No wonder that for years the Beagle has been the most popular hound dog among American pet owners. These are curious, clever, and energetic hounds who require plenty of playtime.";
  char data1[] = "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz";
  char data3[] = "ESP32 is a series of low-cost, low-power system on a chip microcontrollers with integrated Wi-Fi and dual-mode Bluetooth. The ESP32 series employs either a Tensilica Xtensa LX6 microprocessor in both dual-core and single-core variations, an Xtensa LX7 dual-core microprocessor, or a single-core RISC-V microprocessor and includes built-in antenna switches, RF balun, power amplifier, low-noise receive amplifier, filters, and power-management modules.";
  delay(random(500));
  while (true) {
    //delay(random(200));
    int choice = random(1, 4);
    switch (choice) {
      case 1: rc.sendData(data1, strlen(data1)); break;
      case 2: rc.sendData(data2, strlen(data2)); break;
      case 3: rc.sendData(data3, strlen(data3)); break;
      default: Serial.println("choice error");
    }

    byte* recvdata;
    long start = micros();
    float size = rc.receiveData(&recvdata);
    long end = micros();
    long timeTaken = end - start;
    rate = rate * 0.99 + ((size / timeTaken) * 10e3) * 0.01;
    Serial.print(rate);
    Serial.print(",");
    Serial.print(999);
    Serial.println("KBps");
    // for (int c = 0; c < size; c++)
    // {
    //   Serial.print((char)*(recvdata + c));
    // }
    // Serial.println("");
    delete recvdata;
  }
}