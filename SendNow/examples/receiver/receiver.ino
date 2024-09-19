#include <SendNow.h>
#include <cstring>

typedef struct struct_message {
  char a[32];
  int b;
  float c;
  bool d;
} struct_message;

SenderBasic radio;

void setup() {
  radio.init();
  Serial.begin(115200);
}

void loop() {
  struct_message *m;
  int len;
  Serial.println("main loop");
  m = (struct_message*)radio.receive(len);
  if(len != -1)
  {
    Serial.println(m->a);
    Serial.println(m->b);
  }

}
