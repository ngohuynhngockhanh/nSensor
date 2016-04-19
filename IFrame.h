#ifndef __NSENSOR_IFRAME__
#define __NSENSOR_IFRAME__
#include<Arduino.h>
class IFrame {
private:
	char _buffer[20];
public:
	//router id
	byte getRouterID() const {
		return _buffer[0];
	}
	
	byte setRouterID(byte routerID) {
		_buffer[0] = routerID;
	}
	
	//node id
	byte getNodeID() const {
		return _buffer[1];
	}
	
	byte setNodeID(byte nodeID) {
		_buffer[1] = nodeID;
	}
	
	//three last code of pipe
	unsigned long getCodeOfPipe() const {
		unsigned long pipe = (_buffer[4] << 16) | (_buffer[3] << 8) || (_buffer[2]);
		return pipe;
	}
	
	void setCodeOfPipe(unsigned long pipe) {
		_buffer[2] = pipe & 0xFF;
		_buffer[3] = (pipe >> 8) & 0xFF;
		_buffer[4] = (pipe >> 16) & 0xFF;
	}
	//Hopcount
	byte getHopCount() const {
		return _buffer[5];
	}
	
	void setHopCount(byte hopCount) {
		_buffer[5] = hopCount;
	}
	
	bool incHopCount() {
		return (_buffer[5]++ < 255);
	}
	
	
	
	byte setProtocol(byte protocolCode) {
		_buffer[6] &= 0b00000011;//remove old protocol
		_buffer[6] |= protocolCode << 2; 
	}
	
	//markAsTCP
	void markAsTCP() {
		_buffer[6] |= 1 << 1;
	}
	
	//mark as UDP
	void markAsUDP() {
		_buffer[6] &= 0b11111101;
	}
	
	//fragment
	void moreFragment() {
		_buffer[6] |= 1;
	}
	void noMoreFragment() {
		_buffer[6] &= 0b11111110;
	}
	
	const bool isMoreFragment() const {
		return _buffer[6] & 1;
	}
	
	byte getFragmentOffset() const {
		return _buffer[7] >> 3;
	}
	
	byte setFragmentOffset(byte fragmentOffset) {
		_buffer[7] |= fragmentOffset << 3;
	}
	
	//dataLength (sizeof)
	byte getDataLength() const {
		return (_buffer[7] & 7) + 1;//get the last three bits
	}
	
	void setDataLength(byte length) {
		length--;
		_buffer[7] = 0;
		_buffer[7] |= length & 7;
	}
	
	//checksum
	bool getChecksumAt(byte i) const {
		return _buffer[8] & (1 << i);
	}
	
	void clearChecksum() {
		_buffer[8] = 0;
	}
	
	void setCheckSumAt(byte i, bool data) {
		_buffer[8] |= data << i;
	}
	
	bool checkVirifiedData() const {
		for (byte i = 0; i < getDataLength(); i++)
			if ((__builtin_popcount(_buffer[9 + i]) + getChecksumAt(i)) % 2 == 0)
				return false;
		return true;
	}
	
	void setData(void *source, const byte size) {
		char *data = (char *)source;
		clearChecksum();
		for (byte i = 0; i < size; i++) {
			_buffer[9 + i] = data[i];
			setCheckSumAt(i, !(__builtin_popcount(data[i]) % 2));
		}
		setDataLength(size);
	}
	
	//router id - receiver
	void setNodeIDRev(byte nodeID) {
		_buffer[18] = nodeID;
	}
	
	byte getNodeIDRec() const {
		return _buffer[18];
	}

	const char* getBuffer() const {
		return _buffer;
	}
	
	virtual void setOptional() {}
	
	static byte getSize() {
		return 20;
	}
	
	//tcp
	const bool isTCP() const { //false means UDP
		return (_buffer[6] >> 1) & 1;
	}
	const bool isUDP() const { //false means TCP
		return !isTCP();
	}
	
	virtual void setFrame(char *buf) {
		for (byte i = 0; i < getSize(); i++)
			_buffer[i] = buf[i];
	}
	
	//protocol
	const byte getProtocol() const {
		return _buffer[6] >> 2;
	}
	
	char getDataAt(byte i) const {
		if (i >= 0 && i <= 7) {
			return _buffer[9 + i];
		} return 0;
	}
};
#endif
