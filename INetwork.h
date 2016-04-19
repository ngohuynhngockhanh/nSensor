#ifndef __NSENSOR_INETWORK__
#define __NSENSOR_INETWORK__
#include<Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include "config.h"
#include "IFrame.h"
class INetwork: public RF24 {
private:
	uint64_t	_pipe;
protected:
	void setPipe(uint64_t pipe) {
		_pipe = pipe;
	}
	
	void setup() {
		//Init radio
		RF24::begin();	 	
		RF24::setAutoAck(1);	
		RF24::setRetries(3, 3);
		RF24::setDataRate(RF24_1MBPS);		// Tốc độ truyền
		
	}

	
public:
	INetwork(int cePin, int csnPin): RF24(cePin, csnPin) {
		
	}
	
	unsigned long getCodeOfPipe() const {
		uint64_t mask = 0xFFFFFF;
		unsigned long res = getPipe() & mask;
		return res;
	}
	
	unsigned long getCodeOfPipe(uint64_t pipe) const {
		uint64_t mask = 0xFFFFFF;
		unsigned long res = pipe & mask;
		return res;
	}
	
	unsigned long getHeaderOfPipe() const {
		uint64_t mask = 24;
		unsigned long res = getPipe() >> mask;
		return res;
	}
	
	unsigned long getHeaderOfPipe(uint64_t pipe) const {
		uint64_t mask = 24;
		unsigned long res = pipe >> mask;
		return res;
	}
	
	uint64_t getPipe() const {
		return _pipe;
	}
	
	
		
	virtual void writeFrame(IFrame *frame) {
		RF24::stopListening();
		RF24::write(frame->getBuffer(), IFrame::getSize());
		RF24::startListening();
	}
	
	virtual bool readFrame(IFrame *frame) {
		if (RF24::available()) {
			if (DEBUG) Serial.println("Frame received!");
			char *msg = new char[frame->getSize()];
			
			RF24::read(msg, frame->getSize());
			frame->setFrame(msg);
			//debug
			/*
			const char *msg2 = frame->getBuffer();
			for (int i = 0; i < 20; i++) {
			  Serial.print(i);
			  Serial.print(" ");
			  Serial.println((unsigned char)msg2[i], BIN);
			}*/
			delete[]msg;
			return frame->checkVirifiedData();
		} else 
			return false;
	}
	
	uint64_t makePipeFromCodeOfPipe(uint64_t routerPipe, unsigned long codeOfPipe) const {
		routerPipe |= (uint64_t)codeOfPipe;
		return routerPipe;
	}
};
#endif
