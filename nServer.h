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
		RF24::openWritingPipe(PIPE_FOR_REQUEST_ID);
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
	
	void recRequestFromRouterProcess() {
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
	}

	void listening() {
		if (INetwork::readFrame(_frame)) {
			switch (_frame->getProtocol()) {
				case PROTOCOL_REQUEST_TYPE:
					recRequestFromRouterProcess();
					break;
			}
		}
	}
		
	virtual void writeFrame(IFrame *frame) {
		RF24::setAutoAck(frame->isTCP());
		INetwork::writeFrame(frame);
	}
	
	void broastcasting() {
		Request request(getHeaderOfPipe(), getCodeOfPipe(), _currentIDWillBeSupply);
		writeFrame(&request);
	}
};


#endif
