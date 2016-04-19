#include<Arduino.h>
#include "Timer.h"

Timer::Timer() { }

Timer::~Timer() { }



/*
 * Gọi trong hàm setup 1 lần duy nhất
 */
void Timer::initialize()
{
	_lastTick = millis();
}

/*
 * Gọi đầu tiên nhất trong hàm loop()
 */
void Timer::update()
{
	_currentTick = millis();
}

/*
 * Trả về thời gian giữa 2 lần loop()
 * giá trị delta là millisecond
 */
unsigned long Timer::delta()
{
	return _currentTick - _lastTick;
}

/*
 * Yêu cầu gọi cuối cùng trong hàm loop()
 */
void Timer::resetTick()
{
	_lastTick = _currentTick;
}