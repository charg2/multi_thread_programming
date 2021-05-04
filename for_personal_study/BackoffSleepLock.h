#pragma once

#include <cstdint>
#include <Windows.h>
#pragma comment(lib, "Winmm")

#include "backoff.h"

//#include <type_traits>
//
//
//template<typename T = int>
//T fast_rand();
//
//
//template<typename T = int>
//T fast_rand()
//{
//	static_assert(std::is_integral<T>::value, "");
//
//	static int seed = 'fast';
//
//	seed = (214013 * seed + 2531011);
//
//	return (T)((seed >> 16) & 0x7FFF);
//}

//struct BackOffSleep
//{
//public:
//	void do_backoff_sleep();
//
//public:
//	int limit;
//
//	static inline constexpr size_t min_delay{ 1 };
//	static inline constexpr size_t max_delay{ 10 };
//};
//
//
//void BackOffSleep::do_backoff_sleep()
//{
//	int32_t delay = (fast_rand() % limit);
//	if (0 == delay)
//	{
//		return;
//	}
//
//	limit = limit + limit;
//	if (limit > BackOffSleep::max_delay)
//	{
//		limit = BackOffSleep::max_delay;
//	}
//
//	timeBeginPeriod(1);
//	Sleep((uint32_t)min(delay, BackOffSleep::max_delay));
//	timeEndPeriod(1);
//}

#include <cstdint>
#include <atomic>

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
			if ( flag == 0 && InterlockedExchange( (volatile long*) &flag, 1 ) == 0 )
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
