#pragma once

#include "Backoff.h"
#include <Windows.h>

class BackoffLock
{
public:
	BackoffLock() : flag{ 0 }
	{}

	void lock()
	{
		BackOff backoff{ 1000 };

		for (;;)
		{
			if (flag == 0 && InterlockedExchange((volatile LONG*)&flag, 1) == 0)
			{
				break;   
			}
			else
			{
				backoff.do_backoff();
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
