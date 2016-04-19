#ifndef __NSENSOR_NODE__
#define __NSENSOR_NODE__
#include "INetwork.h"
#include "Client.h"
#include "Request.h"
class nNode: public INetwork {
private:
	uint64_t	_routerPipe;
	Client		_client;
	IFrame*		_frame;
protected:

public:
	nNode(int cePin, int csnPin): INetwork(cePin, csnPin) {
		_frame = new IFrame();
	}
	
	void setPipe(uint64_t pipe) {
		INetwork::setPipe(pipe);
	}
	
	void setRouterPipe(uint64_t pipe) {
		_routerPipe = pipe;
	}
	
	void setup() {
		INetwork::setup();
		RF24::openReadingPipe(1, PIPE_FOR_REQUEST_ID);
		RF24::startListening(); 
	}
	
	void recRequestFromRouterProcess() {
		delay(5);//magic code!
		if (DEBUG) Serial.println(F("Send REQUEST feedback"));
		Request *request = (Request*)_frame;
		byte 		routerID	= request->getRouterID();
		byte 		newID		= request->getNewID();
		uint64_t 	routerMAC	= request->getRouterPipe();
		request->setCodeOfPipe(0);
		if (request->incHopCount()) {
			request->setNodeIDRev(request->getNodeID());		//swap rev and sender
			request->setNodeID(newID);
			srand(millis());
			byte data[5];
			data[0] = routerID;
			data[1] = newID;
			for (byte i = 0; i < 3; i++)
				data[2 + i] = (rand() % 255) + 1;
			
			request->setData(data, sizeof(data));
			
			
			RF24::openWritingPipe(routerMAC);	
			
			writeFrame(request);
		}
	}
	
	virtual void writeFrame(IFrame *frame) {
		RF24::setAutoAck(frame->isTCP());
		INetwork::writeFrame(frame);
	}
	
	void run() {	//run the node
		if (INetwork::readFrame(_frame)) {
			switch (_frame->getProtocol()) {
				case PROTOCOL_REQUEST_TYPE:
					recRequestFromRouterProcess();
					break;
			}
		}
	}
};
#endif
