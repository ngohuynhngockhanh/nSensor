#include <SPI.h>
#include <Servo.h>
#include <Node.h>
#include <device.h>
#define EXP               100  // độ chính xác đến 2 chữ số thập phân
nNode node(9, 10);

//Sensor
int ledPin = 7;

unsigned long timer = 0;

void onDevice(byte* source, byte length) {
  Serial.println(F("Rev DEVICE!"));
  for (int i = 0; i < length; i++) {
    Serial.print(F("source["));
    Serial.print(i);
    Serial.print(F("] = "));
    Serial.println(source[i]);
  }
  
  digitalWrite(ledPin, source[0] == 1);
}

void setup() {
  //pinmode luôn phải được gọi đầu tiên!
  pinMode(ledPin, OUTPUT);

  
  Serial.begin(115200);
  Serial.println("Node is online");
  node.setup();
  node.setTrigger(onDevice);  
}


void loop() {
  node.run();
}
