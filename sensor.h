#ifndef __NSENSOR_SENSOR__
#define __NSENSOR_SENSOR__
#include "IFrame.h"

class Sensor: public IFrame {
private:
	word	_factoryCode;
	byte	_routerID;
protected:
	void setFactoryCode() {
		IFrame::setOptional(_factoryCode);
	}
	
	void init() {
		IFrame::setRouterID(_routerID);
		IFrame::setHopCount(0);
		IFrame::setProtocol(PROTOCOL_SENSOR_TYPE);
		IFrame::noMoreFragment();
		IFrame::setFragmentOffset(0);
		IFrame::markAsTCP();
		IFrame::setNodeIDRev(1);		//means router
		
		//missing: nodeID, codeOfPipe, data, factoryCode
	}
	
public:
	
	Sensor(int routerID): _routerID(routerID) {
		init();
	}
	void setFactoryCode(word factoryCode) {
		_factoryCode = factoryCode;
	}
	
	virtual void setOptional() {
		setFactoryCode();
	}
	
	void printCommandHeader() {
		Serial.print(F("SENSOR "));
		Serial.print(_factoryCode);
		Serial.print(F(" "));
	}
	
	void printCommandline() {
		printCommandHeader();
		Serial.print(IFrame::getDataLength());
		for (byte i = 0; i < IFrame::getDataLength(); i++) {
			Serial.print(F(" "));
			Serial.print((byte)IFrame::getDataAt(i));
		}
		Serial.println();
	}
};

#endif