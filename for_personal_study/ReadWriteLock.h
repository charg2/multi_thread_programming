#pragma once

#include <Windows.h>
//#include <process.h>
//#include <memory>

class ReadWriteLock
{
public:
	ReadWriteLock() : flag { }
	{
	}

	ReadWriteLock(const ReadWriteLock& other) = delete;
	ReadWriteLock(ReadWriteLock&& other) = delete;

	void lock()
	{
		for (;;)
		{
			while (flag & LF_WRITE_MASK) // writer lock을 얻고
			{
				YieldProcessor();
			}

			if ( (InterlockedAdd(&flag, LF_WRITE_FLAG) & LF_WRITE_MASK) == LF_WRITE_FLAG )
			{
				while (flag & LF_READ_MASK) // writer lock을 얻은 상태에서 read_lock이 풀릴때까지 검사.
				{
					YieldProcessor();
				}

				return;
			}

			InterlockedAdd(&flag, -LF_WRITE_FLAG);
		}
	}

	void unlock()
	{
		InterlockedAdd(&flag, -LF_WRITE_FLAG);
	}

	void sharedly_lock()
	{

		while (true)
		{
			while (flag & LF_WRITE_MASK)
			{
				YieldProcessor();
			}


			if ( (InterlockedIncrement(&flag) & LF_WRITE_MASK ) == 0 )
			{
				return;
			}
			else
			{
				InterlockedAdd(&flag, -1);
			}
		}
	}

	void sharedly_unlock()
	{
		InterlockedDecrement(&flag);
	}

private:
	enum LockFlag
	{
		LF_WRITE_MASK = 0x7FF00000,
		LF_WRITE_FLAG = 0x00100000,
		LF_READ_MASK = 0x000FFFFF 
	};

private:
	long flag;
};


class ReadWriteLockGuard
{
public:
	ReadWriteLockGuard(ReadWriteLock& lock, bool exclusive_mode) : lock{ lock }, is_exclusive_mode{exclusive_mode}
	{
		if (exclusive_mode)
		{
			lock.lock();
		}
		else
		{
			lock.sharedly_lock();
		}
	};

	~ReadWriteLockGuard()
	{
		if (is_exclusive_mode)
		{
			lock.unlock();
		}
		else
		{
			lock.sharedly_unlock();
		}
	};


private:
	ReadWriteLock& lock;
	bool is_exclusive_mode;
};