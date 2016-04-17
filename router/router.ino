#include <SPI.h>
#include "RF24.h"
const uint64_t pipe = 0xE8E8F0F0E1LL; // địa chỉ để phát
RF24 radio(9,10);
unsigned char msg[1]; //số dương kiểu unsigned char, tương đương với byte msg[1]. Dùng để lưu trữ nội dung sẽ gửi.
const int potpin_1 = A0; // chân biến trở
int val_1;
void setup(){ 
    //========Module NRF24
    Serial.begin(9600);
    radio.begin();                     
    radio.setAutoAck(1);               
    radio.setRetries(1,1);             
    radio.setDataRate(RF24_1MBPS);    // Tốc độ truyền
    radio.setPALevel(RF24_PA_MAX);      // Dung lượng tối đa
    radio.setChannel(10);               // Đặt kênh
    radio.openWritingPipe(pipe);        // mở kênh
}
void loop() { 
    val_1 = analogRead(potpin_1); //đọc giá trị biến trở
    val_1 = map(val_1, 0, 1023, 0, 180); 
    msg[0] = val_1;
    Serial.println(msg[0]);
    radio.write(&msg, sizeof(msg));
}
