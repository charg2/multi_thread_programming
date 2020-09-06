#include "TTASLock.h"
#include "concurrency.h"

TTASLock::TTASLock() : flag{}
{}

void TTASLock::lock()
{
	// ttas
	for (;;)
	{
		if ( flag == 0 && InterlockedExchange((volatile LONG*)&flag, 1) == 0)
		{
			break;
		}
	}
}

void TTASLock::unlock()
{
	flag = 0;
}
