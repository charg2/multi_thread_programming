#pragma once
#include "TTASLock.h"
#include "concurrency.h"

class TASLock
{
public:
	TASLock() : flag{ 0 }
	{}

	void lock()
	{
		for (;;)
		{
			if ( InterlockedExchange((volatile LONG*)&flag, 1) == 0)
			{
				break;
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

