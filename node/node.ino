#include <SPI.h>
#include <Servo.h> 
#include "RF24.h"
const uint64_t pipe = 0xE8E8F0F0E1LL; // địa chỉ phát
RF24 radio(9,10);
unsigned char msg[1];
Servo servo1;// servo1
int data; 
int pos;
    
void setup(){
    Serial.begin(9600);
    radio.begin();                     
    radio.setAutoAck(1);              
    radio.setDataRate(RF24_1MBPS);    // Tốc độ dữ liệu
    radio.setChannel(10);               // Đặt kênh
    radio.openReadingPipe(1,pipe);     
    radio.startListening();            
    servo1.attach(3);  // sử dụng chân số 3 để điều khiển servo
}
    
void loop(){
    if (radio.available()) {
        radio.read(&msg, sizeof(msg));
        servo1.write(msg[0]);
        Serial.println(msg[0]);
    }
}
