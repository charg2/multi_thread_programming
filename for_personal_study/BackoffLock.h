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

//class BackoffSleepLock
//{
//public:
//	BackoffSleepLock() : flag{ 0 }
//	{
//	}
//
//	void lock()
//	{
//		BackOffSleep backoff{ 10 };
//
//		for (;;)
//		{
//			if (flag == 0 && InterlockedExchange((volatile LONG*)&flag, 1) == 0)
//			{
//				break;
//			}
//			else
//			{
//				backoff.do_backoff_sleep();
//			}
//		}
//	}
//
//	void unlock()
//	{
//		flag = 0;
//	}
//
//private:
//	int flag;
//};
//
