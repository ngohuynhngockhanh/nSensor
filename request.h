#ifndef __NSENSOR_REQUEST__
#define __NSENSOR_REQUEST__
#include "IFrame.h"
class Request: public IFrame {
protected:
	
public:
	Request(): IFrame() {
		
	}
	Request(unsigned long headerOfPipe, unsigned long codeOfPipe, byte newID): IFrame() {
		IFrame::setRouterID(ROUTER_ID);
		IFrame::setNodeID(1);			//1 - means router
		IFrame::setCodeOfPipe(codeOfPipe);
		IFrame::setHopCount(0);
		IFrame::setProtocol(PROTOCOL_REQUEST_TYPE);			//0b0000001
		IFrame::noMoreFragment();
		IFrame::setFragmentOffset(0);
		IFrame::markAsTCP();
		uint64_t pipe = headerOfPipe;
		pipe <<= 24;
		pipe |= codeOfPipe;
		char data[7];
		data[0] = ROUTER_ID;
		data[1] = newID;
		for (byte i = 0; i < 5; i++)
			data[6 - i] = (pipe >> (8 * i)) & 0b11111111;
		IFrame::setData(data, sizeof(data));
		IFrame::setNodeIDRev(0);		//0 - means anomyous
	}
	virtual void setOptional() {}	//do nothing
	
	byte getRouterID() const {
		return (byte)IFrame::getDataAt(0);
	}
	
	byte getNewID() const {
		return (byte)IFrame::getDataAt(1);
	}
	
	uint64_t getRouterPipe() {
		uint64_t res = 0;
		for (byte i = 0; i < 5; i++) {
			res |= byte(IFrame::getDataAt(2 + i));
			if (i < 4)
				res <<= 8;
		}
		return res;
	}
	
	unsigned long getNewCodeOfPipe() {
		unsigned long res = 0;
		for (byte i = 0; i < 3; i++) {
			res |= byte(IFrame::getDataAt(2 + i));
			if (i < 4)
				res <<= 8;
		}
		return res;
	}
};

#endif
