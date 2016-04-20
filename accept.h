#ifndef __NSENSOR_ACCEPT__
#define __NSENSOR_ACCEPT__
#include "request.h"
#include "config.h"
class Accept: public Request {
public:
	Accept(): Request() {}
	
	Accept(unsigned long headerOfPipe, unsigned long codeOfPipe, byte newID, uint64_t newPipe): Request(headerOfPipe, codeOfPipe, newID) {
		IFrame::setProtocol(PROTOCOL_ACCEPT_TYPE);
		IFrame::setNodeIDRev(newID);		//because we choose this is our ID
		
		char data[7];
		data[0] = ROUTER_ID;
		data[1] = newID;
		for (byte i = 0; i < 5; i++)
			data[6 - i] = (newPipe >> (8 * i)) & 0b11111111;
		IFrame::setData(data, sizeof(data));
	}
	
	uint64_t getNodePipe() {
		return Request::getRouterPipe();//because we have the same ip
	}
};

#endif