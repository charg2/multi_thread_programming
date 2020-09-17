#pragma once
#include "concurrency.h"

class TTASLock
{
public:
	TTASLock() : flag{}
	{}

	void lock()
	{
		// ttas
		for (;;)
		{
			if (flag == 0 && InterlockedExchange((volatile LONG*)&flag, 1) == 0)
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

