#include <SPI.h>
#include <Servo.h>
#include <Node.h>
nNode node(9, 10);

void setup() {
  Serial.begin(115200);
  Serial.println("Node is online");
  node.setup();
}

void loop() {
  node.run();
}
