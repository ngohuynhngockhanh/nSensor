#ifndef __DAIHUYNH_WORK_SCHEDULER__
#define __DAIHUYNH_WORK_SCHEDULER__
#include <inttypes.h>
class WorkScheduler
{
private:
	unsigned long _ellapsedTime;
	unsigned long _workTime;
	int _workPin;
	void (*func)(int);
	void (*func2)();
public:
	WorkScheduler(int workPin, unsigned long time, void (*func)(int));
	WorkScheduler(unsigned long time, void (*func)());

	~WorkScheduler();

	void update();
};
#endif
