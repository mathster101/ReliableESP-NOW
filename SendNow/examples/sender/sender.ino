#include <SendNow.h>
#include <cstring>

typedef struct struct_message {
  char a[32];
  int b;
  float c;
  bool d;
} struct_message;

// Create a struct_message called myData
struct_message myData;
SenderBasic radio;

void setup() {
  Serial.begin(115200);

  radio.init();
}

void loop() {
  //delay(1000);
  strcpy(myData.a, "THIS IS A CHAR");
  myData.b = random(1, 20);
  myData.c = 1.2;
  myData.d = false;
  radio.send(&myData, sizeof(myData));
  Serial.println("sent!");
}
