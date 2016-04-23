#ifndef __FALLING_BUTTON__
#define __FALLING_BUTTON__
#include<Arduino.h>
class FallingButton {
private:
	byte m_pin;
  byte m_lastState;
public:
	//khởi tạo falling button pin (digital read)
	FallingButton() {}
	FallingButton(const byte pin);
	FallingButton(const FallingButton &button) {
		m_pin = button.getPin();
	}
	
	byte getPin() const {
		return m_pin;
	}
	
	byte check();
};

#endif