#ifndef __NSENSOR_SERVER__
#define __NSENSOR_SERVER__
#include "INetwork.h"
#include "Node.h"
#include "Client.h"
#include "Request.h"
#include "dllist.h"

class Server: public INetwork {
private:
	DList<Client*>	_clientList;
	IFrame*			_frame;
	byte			_currentIDWillBeSupply;
	uint64_t		_currentWritingPipe;
protected:
	uint64_t getCurrentWritingPipe() const {
		return _currentWritingPipe;
	}
public:
	Server(uint64_t pipe, int cePin, int csnPin): _currentIDWillBeSupply(2), INetwork(cePin, csnPin) {
		INetwork::setPipe(pipe);
		_frame = new IFrame();
		_clientList.setMax(MAX_CLIENT_NUMBER);
	}

	word getClientCount() const {
		return _clientList.size();
	}
	
	

	void setup() {
		INetwork::setup();
		RF24::openReadingPipe(1, getPipe());
		RF24::startListening(); 
		_currentWritingPipe = PIPE_FOR_REQUEST_ID;
		RF24::openWritingPipe(_currentWritingPipe);
	}
	
	
	// check if we have a client 
	bool weHaveClient(byte id, uint64_t pipe) {
		for (int i = 0; i < _clientList.size(); i++)
			if (_clientList.at(i)->obj->isThisClient(id, pipe))
				return true;
		return false;
	}
	
	void findNewIDForNextSupply() {
		byte *marked = new byte[MAX_CLIENT_NUMBER];
		byte id;
		for (int i = 0; i < _clientList.size(); i++)
			if ((id = _clientList.at(i)->obj->getID()) < MAX_CLIENT_NUMBER)
				marked[id]++;
			
		for (int i = 2; i < MAX_CLIENT_NUMBER; i++)
			if (marked[i] == 0) {
				_currentIDWillBeSupply = i;
				break;
			}
		delete[]marked;
	}
	
	void recRequestFromClientProcess() {
		if (DEBUG) Serial.println(F("Rev REQUEST feedback"));
		if (!_clientList.canAdd()) {
			if (DEBUG) Serial.println(F("We reached the max, we can't add more"));
			return;
		}
			
		
		Request *request = (Request *)_frame;
		byte 			routerID	= request->getRouterID();
		byte 			newID		= request->getNewID();
		unsigned long 	newCodePipe	= request->getNewCodeOfPipe();
		uint64_t		newPipe		= makePipeFromCodeOfPipe(getPipe(), newCodePipe);
		
		//check router has client list
		if (weHaveClient(newID, newPipe)) {
			if (DEBUG) Serial.println(F("We have already had this Client!"));
			return;
		}
		
		if (DEBUG) Serial.println(F("Let append new Client"));
		Client *client = new Client(newID, newPipe);
		_clientList.push_back(client);
		findNewIDForNextSupply();
		
		Accept accept(getHeaderOfPipe(), getCodeOfPipe(), newID, newPipe);
		INetwork::writeFrame(&accept);
	}
	
	void recSensorFromClientProcess() {
		if (DEBUG) Serial.println(F("Rev SENSOR frame!"));
		if (_frame->getNodeIDRev() == 1) {
			if (DEBUG) Serial.println(F("Send SENSOR command to controller!"));
			Sensor *sensor = (Sensor *)_frame;
			sensor->printCommandline();
		} else {
			//need to code
		}		
	}
	
	void recDeviceFromClientProcess() {
		if (DEBUG) Serial.println(F("Rev DEVICE frame!"));
		
	}

	void listening() {
		if (INetwork::readFrame(_frame)) {
			if (_frame->getRouterID() != ROUTER_ID)
				if (DEBUG)
					Serial.println(F("Repeat to other router!"));
			switch (_frame->getProtocol()) {
				case PROTOCOL_REQUEST_TYPE:
					recRequestFromClientProcess();
					break;
				case PROTOCOL_SENSOR_TYPE:
					recSensorFromClientProcess();
					break;
				case PROTOCOL_DEVICE_TYPE:
					recDeviceFromClientProcess();
					break;
			}
		}
	}
	
	void writeFrameRemember(IFrame *frame, uint64_t pipe) {
		if (DEBUG)
			Serial.println(F("Write frame and remember"));
		_currentWritingPipe = pipe;
		RF24::openWritingPipe(_currentWritingPipe);
		writeFrame(frame);
	}
		
	virtual void writeFrame(IFrame *frame) {
		RF24::setAutoAck(frame->isTCP());
		INetwork::writeFrame(frame);
	}
	
	bool sendToNode(byte nodeID, IFrame *frame) {
		for (int i = 0; i < _clientList.size(); i++)
			if (_clientList.at(i)->obj->getID() == nodeID) {
				if (DEBUG) {
					Serial.print(F("Send to node "));
					Serial.println(nodeID);
				}
				frame->setCodeOfPipe(getCodeOfPipe());
				writeFrameRemember(frame, _clientList.at(i)->obj->getPipe());
				return true;
			}
		return false;
	}
	
	void broastcasting() {
		Request request(getHeaderOfPipe(), getCodeOfPipe(), _currentIDWillBeSupply);
		RF24::openWritingPipe(PIPE_FOR_REQUEST_ID);
		INetwork::writeFrame(&request);
		RF24::openWritingPipe(_currentWritingPipe);
	}
};


#endif
