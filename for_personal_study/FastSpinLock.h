#pragma once


#include <Windows.h>

class FastSpinLock
{
public:
	FastSpinLock() : mLockFlag{ 0 }
	{
	}


	~FastSpinLock()
	{
	}


	void EnterLock()
	{
		for (int nloops = 0; ; nloops++)
		{
			if (InterlockedExchange(&mLockFlag, 1) == 0)
				return;

			timeBeginPeriod(1);
			Sleep((DWORD)min(10, nloops));
			timeEndPeriod(1);
		}
	}

	void LeaveLock()
	{
		mLockFlag = 0;
	}

private:
	long mLockFlag;
};


