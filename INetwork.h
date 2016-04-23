#ifndef __NSENSOR_INETWORK__
#define __NSENSOR_INETWORK__
#include<Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <EEPROMex.h>
#include "config.h"
#include "IFrame.h"
#include "FallingButton.h"

void(* resetFunc) (void) = 0; //declare reset function @ address 0
class INetwork: public RF24 {
private:
	uint64_t		_pipe;
	FallingButton	buttonReset;
protected:
	void setPipe(uint64_t pipe) {
		_pipe = pipe;
	}
	
	void setup() {
		//Init radio
		if (DEBUG) Serial.println(F("EEPROM is ready!")); 	
		RF24::setAutoAck(1);	
		RF24::setRetries(3, 3);
		RF24::setDataRate(RF24_1MBPS);		// Tốc độ truyền
		
		uint64_t pipe = EEPROM.readByte(0);
		pipe <<= 32;
		pipe |= (uint64_t)EEPROM.readLong(1);
		if (pipe != 0) {
			setPipe(pipe);
			RF24::openReadingPipe(1, pipe);
		} 
	}

	
public:
	INetwork(int cePin, int csnPin): RF24(cePin, csnPin) {
		buttonReset = FallingButton(RESET_PIN);
	}
	
	void resetWork() {
		if (buttonReset.check()) {
			if (DEBUG)
				Serial.println(F("Reseting"));
			//remove eeprom
			removeEEPROM();
			delay(1000);
			resetFunc();
		}
	}
	
	unsigned long getCodeOfPipe() const {
		uint64_t mask = 0xFFFFFFL;
		unsigned long res = getPipe() & mask;
		return res;
	}
	
	unsigned long getCodeOfPipe(uint64_t pipe) const {
		uint64_t mask = 0xFFFFFFL;
		long res = pipe & mask;
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
		uint64_t code = codeOfPipe;
		uint64_t res = (long)routerPipe;
		res >>= 24;
		res <<= 24;
		res |= code;
		return res;
	}
	
	virtual void saveEEPROM() {
		EEPROM.writeByte(0, byte(_pipe >> 32));
		EEPROM.writeLong(1, _pipe & 0xFFFFFFFFLL);
	}
	
	virtual void removeEEPROM() {
		EEPROM.writeByte(0, 0);
		EEPROM.writeLong(1, 0);
	}
};
#endif
