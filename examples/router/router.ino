#include<Timer.h>
#include<WorkScheduler.h>
#include<nServer.h>
#include<SerialCommand.h>
#include<Device.h>
//config

//Kernel
Server server(PIPE, 2, 3);
SerialCommand sCmd; // Khai báo biến sử dụng thư viện Serial Command

WorkScheduler *receiverWorkSchduler;
WorkScheduler *broastcastingWorkScheduler;
//DEBUG
WorkScheduler *ramManagerWorkScheduler;

void receiver() {
  server.listening();
}

void broastcasting() {
  Serial.println(F("Broastcasting"));
  server.broastcasting();
  
}

void device_listening() {
  char *arg;
  arg = sCmd.next();//read nodeID
  byte nodeID = atoi(arg);

  arg = sCmd.next();//read data length
  byte dataLength = atoi(arg);

  byte *source = new byte[dataLength];
  for (byte i = 0; i < dataLength; i++)
    source[i] = atoi(sCmd.next());

  Device device(ROUTER_ID);
  device.setData(nodeID, source, dataLength);
  delete[]source;
  if (server.sendToNode(nodeID, &device)) 
    Serial.println(F("AT 1"));
  else
    Serial.println(F("AT 0"));
}

//DEBUG
void ramManager() {
  // Trong trường hợp này, ta có thể hiểu extern sẽ khai báo một biến toàn cục trong chương trình (nếu chưa có) hoặc include một biến toàn cục đã được extern trước đó
  extern int __heap_start;
  extern int *__brkval; 
  
  //Dấu & phía trước tên biến / tên con trỏ sẽ cho ta biết vị trí ô nhớ mà nó đang đứng
  //Lưu ý: bài viết này không dành cho beginner và bạn cần tưởng tượng một chút để có thể mườn tượng vấn đề
  int freeRam = (int) SP - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
  Serial.print(F("RAM "));
  Serial.println(freeRam);
}
//User






void setup(){ 
  //Khởi gạo class timer (design pattern singleton) - bắt buộc phải có trong hàm setup (trước khi khởi tạo các job)
  Timer::getInstance()->initialize();
  //========Module NRF24
  Serial.begin(115200);
  server.setup();
  receiverWorkSchduler        = new WorkScheduler(0UL, receiver);
  broastcastingWorkScheduler  = new WorkScheduler(WORKSCHEDULER_BROASTCASTING_CYCLE_TIME, broastcasting);
  ramManagerWorkScheduler     = new WorkScheduler(5000UL, ramManager);

  sCmd.addCommand("DEVICE",   device_listening);
  //DEBUG
  
  Serial.println(F("Router is online!"));
}
void loop() { 
  sCmd.readSerial();
  Timer::getInstance()->update();
  receiverWorkSchduler->update();
  broastcastingWorkScheduler->update();
  ramManagerWorkScheduler->update();
  server.resetWork();
  Timer::getInstance()->resetTick();
}
