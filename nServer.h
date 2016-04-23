#ifndef __NSENSOR_SERVER__
#define __NSENSOR_SERVER__
#include "INetwork.h"
#include "Node.h"
#include "Client.h"
#include "Request.h"

class Server: public INetwork {
private:
	Client*			_clientList[MAX_CLIENT_NUMBER + 1];
	byte			_clientCount;
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
		for (int i = 0; i <= MAX_CLIENT_NUMBER; i++) {
			_clientList[i] = NULL;
		}
		_clientList[1] = new Client(1, pipe);
		_clientCount = 2;//0 and 1
	}

	word getClientCount() const {
		return _clientCount;
	}
	
	

	void setup() {
		RF24::begin();	
		_currentWritingPipe = PIPE_FOR_REQUEST_ID;
		RF24::openWritingPipe(_currentWritingPipe);
		INetwork::setup();
		RF24::openReadingPipe(1, getPipe());
		RF24::startListening(); 
		loadEEPROM();
	}
	
	
	// check if we have a client 
	bool weHaveClient(byte id, uint64_t pipe) {
		for (int i = 1; i <= MAX_CLIENT_NUMBER; i++)
			if (_clientList[i] != NULL && _clientList[i]->isThisClient(id, pipe)) {
				if (DEBUG) {
					Serial.print(F("That is "));
					Serial.println(i);
				}					
				return true;
			}
				
		return false;
	}
	
	const bool canAddClient() const {
		return _clientCount <= MAX_CLIENT_NUMBER;
	}
	
	void findNewIDForNextSupply() {
		for (int i = 2; i <= MAX_CLIENT_NUMBER; i++)
			if (_clientList[i] == NULL) {
				_currentIDWillBeSupply = i;
				return;
			}		
	}
	
	void recRequestFromClientProcess() {
		if (DEBUG) Serial.println(F("Rev REQUEST feedback"));
		if (!canAddClient()) {
			if (DEBUG) Serial.println(F("We reached the max, we can't add more"));
			return;
		}
			
		
		Request *request = (Request *)_frame;
		byte 			routerID	= request->getRouterID();
		byte 			newID		= request->getNewID();
		unsigned long 	newCodePipe	= request->getNewCodeOfPipe();
		uint64_t		newPipe		= makePipeFromCodeOfPipe(getPipe(), newCodePipe);
		if (DEBUG) {
			Serial.print(F("Router id "));
			Serial.println(routerID);
			Serial.print(F("New NodeID: "));
			Serial.println(newID);
		}
		//check router has client list
		if (weHaveClient(newID, newPipe)) {
			if (DEBUG) {
				Serial.println(F("We have already had this Client!"));
				Serial.println((long)getCodeOfPipe(newPipe));
				Serial.println(newID);
			}
			return;
		}
		
		if (DEBUG) Serial.println(F("Let append new Client"));
		Client *client = new Client(newID, newPipe);
		_clientList[_currentIDWillBeSupply] = client;
		findNewIDForNextSupply();
		
		Accept accept(getHeaderOfPipe(), getCodeOfPipe(), newID, newPipe);
		writeFrameRemember(&accept, PIPE_FOR_REQUEST_ID);
		_clientCount++;
		
		saveEEPROMClient(client->getClientInfo(), newID);
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
		if (nodeID == 0 || nodeID > _clientCount) return false;
		if (DEBUG) {
			Serial.print(F("Send to node "));
			Serial.println(nodeID);
		}
		frame->setCodeOfPipe(getCodeOfPipe());
		writeFrameRemember(frame, _clientList[nodeID]->getPipe());
		return true;
	}
	
	void broastcasting() {
		Request request(getHeaderOfPipe(), getCodeOfPipe(), _currentIDWillBeSupply);
		RF24::openWritingPipe(PIPE_FOR_REQUEST_ID);
		INetwork::writeFrame(&request);
		RF24::openWritingPipe(_currentWritingPipe);
	}
	
	void saveEEPROMClient(const ClientInfo &info, byte id) {
		if (id <= 1) return; //0 - means anomyous, 1 - means router
		id -= 2;
		
		if (DEBUG) {
			Serial.print(F("A block has "));
			Serial.print(sizeof(ClientInfo));
			Serial.println(F(" bytes"));
		}
			
		
		//save client Count
		EEPROM.writeByte(5, _clientCount);
		EEPROM.writeBlock(6 + (sizeof(ClientInfo) * id), info);
	}
	
	void loadEEPROM() {
		_clientCount = EEPROM.readByte(5);
		for (byte i = 2; i < _clientCount; i++) {
			ClientInfo info;
			EEPROM.readBlock(6 + (sizeof(ClientInfo) * (i - 2)), info);
			if (info.id == 0)
				continue;
			_clientList[i] = new Client(info);
		}
		findNewIDForNextSupply();
		if (DEBUG) {
			Serial.print(F("We have loaded "));
			Serial.print(_clientCount);
			Serial.println(F(" client to the list!"));
		}
	}
	
	virtual void removeEEPROM() {
		INetwork::removeEEPROM();
		ClientInfo info = {0, 0};
		EEPROM.writeByte(5, 0);
		for (byte i = 2; i < _clientCount; i++)
			EEPROM.writeBlock(6 + (sizeof(ClientInfo) * (i - 2)), info);
		
	}
};


#endif
