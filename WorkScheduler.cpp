#include<Arduino.h>
#include "Timer.h"
#include "WorkScheduler.h"

WorkScheduler::WorkScheduler(int workPin, unsigned long time, void (*func)(int))
{
	_workPin = workPin;
	_workTime = time;
	_ellapsedTime = 0;
	this->func = func;
}

WorkScheduler::WorkScheduler(unsigned long time, void (*func)()): _workTime(time), func2(func), _ellapsedTime(0), _workPin(-1) {}

WorkScheduler::~WorkScheduler()
{
	_workPin = 0;
	_workTime = 0;
	_ellapsedTime = 0;
	func = NULL;
	func2= NULL;
}

void WorkScheduler::update()
{
	_ellapsedTime += Timer::getInstance()->delta();
	if (_ellapsedTime >= _workTime)
	{
		_ellapsedTime -= _workTime;
		if (func != NULL)
		{
			if (_workPin == -1)
				func2();
			else
				func(_workPin);
		} else if (func2 != NULL)
      func2();
	}
}
