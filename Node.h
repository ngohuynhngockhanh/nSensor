#ifndef __NSENSOR_NODE__
#define __NSENSOR_NODE__
#include "INetwork.h"
#include "Client.h"
#include "request.h"
#include "accept.h"
#include "sensor.h"
#include "device.h"
class nNode: public INetwork {
private:
	byte 		_nodeID;
	uint64_t	_routerPipe;
	Client		_client;
	IFrame*		_frame;
	bool		_isActive;
	void 		(*_trigger)(byte*, byte);
	bool 		_isSetTrigger;
protected:
	void setActive() {
		_isActive = true;
	}
	
	void setDeactuve() {
		_isActive = false;
	}
	
	void setNodeID(byte nodeID) {
		_nodeID = nodeID;
	}
public:
	nNode(int cePin, int csnPin): _isSetTrigger(false), _nodeID(0), INetwork(cePin, csnPin) {
		setPipe(PIPE_FOR_REQUEST_ID);
		_frame = new IFrame();
	}
	
	const bool isSetTrigger() const {
		return _isSetTrigger;
	}
	
	void setTrigger(void (*trigger)(byte*, byte)) {
		_trigger = trigger;
		_isSetTrigger = true;
	}
	
	const byte getNodeID() const {
		return _nodeID;
	}
	
	const bool isActive() const {
		return _isActive;
	}
	
	void setPipe(uint64_t pipe) {
		INetwork::setPipe(pipe);
	}
	
	void setRouterPipe(uint64_t pipe) {
		_routerPipe = pipe;
	}
	
	const uint64_t getRouterPipe() const {
		return _routerPipe;
	}
	
	void setup() {
		RF24::begin();	
		RF24::openReadingPipe(1, getPipe());
		INetwork::setup();		
		RF24::startListening(); 
		
		byte nodeID = EEPROM.readByte(5);
		if (nodeID != 0 && getPipe() != PIPE_FOR_REQUEST_ID) {//restore state
			setNodeID(nodeID);
			setActive();
			
			uint64_t routerPipe = EEPROM.readByte(6);
			routerPipe <<= 32;
			routerPipe |= (uint64_t)EEPROM.readLong(7);
			if (DEBUG) {
				Serial.print(F("NodeID = "));
				Serial.println(nodeID);
				Serial.print((long)getHeaderOfPipe(routerPipe), HEX);
				Serial.println((long)getCodeOfPipe(routerPipe), HEX);
				_routerPipe = routerPipe;
				RF24::openWritingPipe(_routerPipe);
				Serial.println(F("RESTORE old status"));
			}
				
		}
	}
	
	virtual void saveEEPROM() {
		INetwork::saveEEPROM();
		EEPROM.writeByte(5, _nodeID);
		EEPROM.writeByte(6, byte(_routerPipe >> 32));
		EEPROM.writeLong(7, _routerPipe & 0xFFFFFFFFLL);
	}
	
	virtual void removeEEPROM() {
		INetwork::removeEEPROM();
		EEPROM.writeByte(5, 0);
		EEPROM.writeByte(6, 0);
		EEPROM.writeLong(7, 0);
	}
	
	void recRequestFromRouterProcess() {
		if (!isActive()) {
			delay(5);//magic code!
			if (DEBUG) {
				Serial.println(F("Send REQUEST feedback"));
				Serial.println(_nodeID);
				Serial.println((long)getCodeOfPipe());
			}
			Request *request = (Request*)_frame;
			byte 		routerID	= request->getRouterID();
			byte 		newID		= request->getNewID();
			uint64_t 	routerPipe	= request->getRouterPipe();
			_routerPipe				= routerPipe;
			if (DEBUG) {
				Serial.print(F("Router id "));
				Serial.println(routerID);
				Serial.print(F("New NodeID: "));
				Serial.println(newID);
			}
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
				RF24::openWritingPipe(routerPipe);	
				
				writeFrame(request);
			}
		}
	}
	
	void recAcceptFromRouterProcess() {
		if (isActive())
			return;
		if (DEBUG) Serial.println(F("Rev ACCEPT feedback"));
		Accept *accept = (Accept *)_frame;
		byte 		routerID	= accept->getRouterID();
		byte 		newID		= accept->getNewID();
		uint64_t 	nodePipe	= accept->getNodePipe();
		
		setActive();
		RF24::openReadingPipe(1, nodePipe);
		setPipe(nodePipe);
		_nodeID					= newID;
		saveEEPROM();
	}
	
	void recDeviceFromRouterProcess() {
		if (!isSetTrigger())
			return;
		Device *device = (Device *)_frame;
		byte *source = (byte*)device->getData();
		_trigger(source, device->getDataLength());
		delete []source;		
	}
	
	virtual void writeFrame(IFrame *frame) {
		RF24::setAutoAck(frame->isTCP());
		INetwork::writeFrame(frame);
	}
	
	
	void run() {	//run the node
		if (INetwork::readFrame(_frame)) {
			if ((_frame->getNodeIDRev() == _nodeID && isActive()) || (_frame->getNodeIDRev() == 0 && !isActive()) || (_frame->getNodeIDRev() != 0 && !isActive() && _frame->getProtocol() == PROTOCOL_ACCEPT_TYPE))
				switch (_frame->getProtocol()) {
					case PROTOCOL_REQUEST_TYPE:
						recRequestFromRouterProcess();
						break;
					case PROTOCOL_ACCEPT_TYPE:
						recAcceptFromRouterProcess();
						break;
					case PROTOCOL_DEVICE_TYPE:
						recDeviceFromRouterProcess();
						break;
				}
			else {
				if (DEBUG) 
					Serial.println(F("Wrong nodeid code!"));
				return;
			}
		}
		resetWork();
	}
};
#endif
