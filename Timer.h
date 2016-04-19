#ifndef __DAIHUYNH_TIMER__
#define __DAIHUYNH_TIMER__
#include <inttypes.h>
class Timer
{
private:
	unsigned long _lastTick;
	unsigned long _currentTick;

	Timer();

	~Timer();
public:
	static Timer* getInstance()
	{
		static Timer* instance = new Timer();
		return instance;
	}

	/*
	 * Gọi trong hàm setup 1 lần duy nhất
	 */
	void initialize();

	/*
	 * Gọi đầu tiên nhất trong hàm loop()
	 */
	void update();

	/*
	 * Trả về thời gian giữa 2 lần loop()
	 * giá trị delta là millisecond
	 */
	unsigned long delta();

	/*
	 * Yêu cầu gọi cuối cùng trong hàm loop()
	 */
	void resetTick();
};

#endif