#include <ReliableEspNow.h>

reliableConnection rc;

char dataBuffer[2000];

void setup() {
  Serial.begin(921600);
  rc.init();
  delay(random(0, 500));
  Serial.println("setup done");
}

void loop() {

  char dataDBG[] = "Not only is the Beagle an excellent hunting dog and loyal companion, it is also happy-go-lucky, funny, and thanks to its pleading expression cute. They were bred to hunt in packs, so they enjoy company and are generally easygoing. There are two Beagle varieties: those standing under 13 inches at the shoulder, and those between 13 and 15 inches. Both varieties are sturdy, solid, and 'big for their inches,' as dog folks say. They come in such pleasing colors as lemon, red and white, and tricolor. The Beagle's fortune is in his adorable face, with its big brown or hazel eyes set off by long, houndy ears set low on a broad head. A breed described as 'merry' by its fanciers, Beagles are loving and lovable, happy, and companionable'all qualities that make them excellent family dogs. No wonder that for years the Beagle has been the most popular hound dog among American pet owners. These are curious, clever, and energetic hounds who require plenty of playtime.";

  if (Serial.available() > 0) {
    int numBytes = Serial.readBytesUntil('\n', dataBuffer, 1501);
    dataBuffer[numBytes + 1] = '\0';
    rc.sendData(dataBuffer, strlen(dataBuffer));
    Serial.println("<sent>");
  }

  byte* recvdata;
  int size = rc.receiveData(&recvdata);
  if (size > 0) {
    
    for (int c = 0; c < size; c++) {
      Serial.print((char)*(recvdata + c));
    }
    Serial.print("\n");
  }
  delete recvdata;
}