#ifndef __NSENSOR_CLIENT__
#define __NSENSOR_CLIENT__
#include<Arduino.h>
struct ClientInfo {
	byte id;
	uint64_t pipe;
};
class Client {
private:
	byte		_id;
	uint64_t	_pipe;
public:
	Client() {}
	Client(byte id, uint64_t pipe): _id(id), _pipe(pipe) {}
	Client(const ClientInfo& info): _id(info.id), _pipe(info.pipe) {}
	//get id
	byte getID() const {
		return _id;
	}
	
	//get pipe
	uint64_t getPipe() const {
		return _pipe;
	}
	
	//set id
	void setID(byte id) {
		_id = id;
	}
	
	//set pipe
	void setPipe(uint64_t pipe) {
		_pipe = pipe;
	}
	
	//check is this client
	bool isThisClient(byte id, uint64_t pipe) {
		return _id == id || (long long)_pipe == (long long)pipe;
	}
	
	ClientInfo getClientInfo() const {
		ClientInfo info = {_id, _pipe};
		return info;
	}
};
#endif