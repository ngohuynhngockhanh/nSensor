#ifndef __NSENSOR_DEVICE__
#define __NSENSOR_DEVICE__
#include "IFrame.h"

class Device: public IFrame {
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
		IFrame::setProtocol(PROTOCOL_DEVICE_TYPE);
		IFrame::noMoreFragment();
		IFrame::setFragmentOffset(0);
		IFrame::markAsTCP();
		IFrame::setNodeID(1);		//means router
		
		//missing: nodeID, codeOfPipe, data, factoryCode
	}
	
public:
	
	Device(int routerID): _routerID(routerID) {
		init();
	}
	void setFactoryCode(word factoryCode) {
		_factoryCode = factoryCode;
	}
	
	virtual void setOptional() {
		setFactoryCode();
	}

	void setData(byte nodeID, byte *source, const byte size) {
		IFrame::setData((char *)source, size);
		IFrame::setNodeIDRev(nodeID);
	}
};

#endif