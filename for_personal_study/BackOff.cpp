#include "fastrand.h"
#include "backoff.h"

#include <Windows.h>


void BackOff::do_backoff()
{
	int32_t delay = ( fast_rand() % limit );
	if (0 == delay)
	{
		return;
	}

	limit = limit + limit;
	if (limit > max_delay)
	{
		limit = max_delay;
	}

#if _M_AMD64
	spin_wait(delay);
#else  // x86
	_asm
	{
		mov ecx, delay;
	}
custom_loop:
	_asm
	{
		loop custom_loop;
	}
#endif
}

void BackOffSleep::do_backoff_sleep()
{
	int32_t delay = ( fast_rand() % limit );
	if ( 0 == delay )
	{
		return;
	}

	limit = limit + limit;
	if ( limit > BackOffSleep::max_delay )
	{
		limit = BackOffSleep::max_delay;
	}

	timeBeginPeriod( 1 );
	Sleep( ( uint32_t )min( delay, BackOffSleep::max_delay ) );
	timeEndPeriod( 1 );
}
