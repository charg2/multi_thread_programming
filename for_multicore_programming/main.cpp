#include <iostream>
#include <thread>
#include <vector>

#include "ALock.h"
#include "TTASLock.h"
#include "TASLock.h"
#include "CLHLock.h"

using namespace std;

constexpr size_t test_thread_count = 16;
constexpr size_t test_count = 400000;

ALock		a_lock(test_thread_count);
TTASLock	ttas_lock;
TASLock		tas_lock;
CLHLock		clh_lock;

size_t g_counter {};

void test_alock()
{
	for (int n{}; n < test_count; ++ n)
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

void test_procedure_base( void(*lock_procedure)(), const string_view& str_v )
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

struct testing_context
{
	void (*lock_procedure)();
	std::string_view text;
};


auto main() -> void
{
	std::cout << __FILE__ << std::endl;

	std::vector<testing_context> tests;
	tests.push_back({ test_taslock, "tas_lock" });
	tests.push_back({ test_ttaslock, "ttas_lock" });
	tests.push_back({ test_alock, "alock" });
	tests.push_back({ test_clhlock, "clhlock" });

	for( auto test : tests)
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

