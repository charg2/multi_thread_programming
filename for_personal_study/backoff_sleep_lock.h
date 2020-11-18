#pragma once

#include <cstdint>
#include <Windows.h>
#pragma comment(lib, "Winmm")

#include "backoff.h"

class BackoffSleepLock
{
public:
	BackoffSleepLock() : flag{ 0 }
	{
	}

	void lock()
	{
		BackOffSleep backoff{ 10 };

		for (;;)
		{
			if (flag == 0 && InterlockedExchange((volatile LONG*)&flag, 1) == 0)
			{
				break;
			}
			else
			{
				backoff.do_backoff_sleep();
			}
		}
	}

	void unlock()
	{
		flag = 0;
	}

private:
	int flag;
};
