#pragma once
#include <cstdint>
#pragma comment(lib, "Winmm")

#if _M_AMD64
extern "C" int __stdcall spin_wait(uint64_t limit);
#endif

struct BackOff
{
public:
	void do_backoff();

public:
	int limit;	

	static inline constexpr size_t min_delay{ 100 };
	static inline constexpr size_t max_delay{ 1000 };
};



struct BackOffSleep
{
public:
	void do_backoff_sleep();

public:
	int limit;

	static inline constexpr size_t min_delay{ 1 };
	static inline constexpr size_t max_delay{ 10 };
};


