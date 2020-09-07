#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

#include "ALock.h"
#include "TTASLock.h"
#include "TASLock.h"
#include "CLHLock.h"
#include "MCSLock.h"

using namespace std;

constexpr size_t test_thread_count = 16;
constexpr size_t test_count = 3'200'000 / test_thread_count;

ALock				a_lock(test_thread_count);
TTASLock			ttas_lock;
TASLock				tas_lock;
CLHLock				clh_lock;
MCSLock				mcs_lock;
mutex				mtx;
CRITICAL_SECTION	critical_section;
SRWLOCK				rw_lock;

size_t g_counter {};

void initialize();

void test_mutex();
void test_critical_section();
void test_rw_lock();
void test_alock();
void test_ttaslock();
void test_taslock();
void test_clhlock();
void test_mcslock();
void test_procedure_base(void(*lock_procedure)(), const string_view& str_v);


struct testing_context
{
	void (*lock_procedure)();
	std::string_view text;
};


auto main() -> void
{
	std::cout << __FILE__ << std::endl;

	initialize();

	std::vector<testing_context> tests;

	tests.push_back({ test_taslock, "tas_lock" });
	tests.push_back({ test_ttaslock, "ttas_lock" });
	tests.push_back({ test_mutex, "mutex" });
	tests.push_back({ test_alock, "alock" });
	tests.push_back({ test_clhlock, "clhlock" });
	tests.push_back({ test_mcslock, "mcslock" });
	tests.push_back({ test_rw_lock, "rw_lock" });
	tests.push_back({ test_critical_section, "critical_section" });

	for( const auto& test : tests)
	{
		test_procedure_base( test.lock_procedure, test.text );
	}

	return; 
}


/*
tas_lock counter : 6400000 delta_tick : 6062
ttas_lock counter : 6400000 delta_tick : 5829
alock counter : 6400000 delta_tick : 5453
clhlock counter : 6400000 delta_tick : 2437
*/


void initialize()
{
	InitializeCriticalSection(&critical_section);
	InitializeSRWLock(&rw_lock);
}

void test_mutex()
{
	for (int n{}; n < test_count; ++n)
	{
		mtx.lock();

		g_counter += 1;

		mtx.unlock();
	}
}

void test_critical_section()
{
	for (int n{}; n < test_count; ++n)
	{
		EnterCriticalSection(&critical_section);

		g_counter += 1;

		LeaveCriticalSection(&critical_section);
	}
}

void test_rw_lock()
{
	for (int n{}; n < test_count; ++n)
	{
		AcquireSRWLockExclusive(&rw_lock);

		g_counter += 1;

		ReleaseSRWLockExclusive(&rw_lock);
	}
}


void test_alock()
{
	for (int n{}; n < test_count; ++n)
	{
		a_lock.lock();

		g_counter += 1;

		a_lock.unlock();
	}
}

void test_ttaslock()
{
	for (int n{}; n < test_count; ++n)
	{
		ttas_lock.lock();

		g_counter += 1;

		ttas_lock.unlock();
	}
}

void test_taslock()
{
	for (int n{}; n < test_count; ++n)
	{
		tas_lock.lock();

		g_counter += 1;

		tas_lock.unlock();
	}
}


void test_clhlock()
{
	for (int n{}; n < test_count; ++n)
	{
		clh_lock.lock();

		g_counter += 1;

		clh_lock.unlock();
	}
}


void test_mcslock()
{
	for (int n{}; n < test_count; ++n)
	{
		mcs_lock.lock();

		g_counter += 1;

		mcs_lock.unlock();
	}
}



void test_procedure_base(void(*lock_procedure)(), const string_view& str_v)
{
	std::vector<std::thread> ths;
	ths.reserve(test_thread_count);

	size_t start_tick = GetTickCount64();

	for (int n{}; n < test_thread_count; ++n)
	{
		ths.emplace_back(lock_procedure);
	}

	for (auto& th : ths)
	{
		th.join();
	}

	size_t delta_tick = GetTickCount64() - start_tick;

	std::cout << str_v << " counter : " << g_counter << " delta_tick : " << delta_tick << endl;

	g_counter = 0;
}
