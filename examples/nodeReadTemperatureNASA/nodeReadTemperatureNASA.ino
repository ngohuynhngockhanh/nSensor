#include <SPI.h>
#include <Servo.h>
#include <Node.h>
#include <Sensor.h>
#define EXP               100  // độ chính xác đến 2 chữ số thập phân
nNode node(9, 10);
Sensor *sensor;

//Sensor
int sensorPin = A0;

unsigned long timer = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  Serial.println("Node is online");
  node.setup();
}

void parse_float_temperature(float val, byte &prefix, byte &suffix) {
  prefix = byte(val);
  suffix = (val - prefix) * EXP;
}

void loop() {
  node.run();
  if (node.isActive()) {
    static byte i = 0;
    if (i == 0) {
      Serial.println("Node has been accepted!");
      sensor = new Sensor(node.getRouterPipe());
      sensor->setFactoryCode(0x0001);
      sensor->setNodeID(node.getNodeID());
      sensor->setCodeOfPipe(node.getCodeOfPipe());
      i = 1;
    }

    if (millis() - timer > 1000) {
      //http://arduino.vn/bai-viet/296-cam-bien-nhiet-do-lm35-va-cach-su-dung-no-trong-moi-truong-arduino
      int reading = analogRead(sensorPin);  
      float voltage = reading * 5.0 / 1024.0;
      float temp = voltage * 100.0;

      //http://arduino.vn/bai-viet/408-mot-chuoi-voi-noi-dung-duoc-quy-dinh-san-trong-arduino-formatted-string
      
      byte data[2];
      parse_float_temperature(temp, data[0], data[1]);
      sensor->setData(data, sizeof(data));
      node.writeFrame(sensor);

      timer = millis();
    }
  }
}
