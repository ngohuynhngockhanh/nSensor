#include<Arduino.h>
#include "FallingButton.h"

FallingButton::FallingButton(const byte pin): m_pin(pin), m_lastState(HIGH) {//luôn luôn là HIGH cho lần đầu khởi tạo vì ta dùng theo thiết kế INPUT_PULLUP
  pinMode(pin, INPUT_PULLUP);
}


byte FallingButton::check() {
  byte nowState = digitalRead(m_pin);
  byte res = (m_lastState == HIGH && nowState == LOW);
  m_lastState = nowState;
  return res;
}
