#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <string_view>

#include "ALock.h"
#include "TTASLock.h"
#include "TASLock.h"
#include "CLHLock.h"
#include "MCSLock.h"
#include "TOLock.h"
#include "BackoffLock.h"
#include "ReadWriteLock.h"
#include "FastSpinLock.h"

using namespace std;

constexpr size_t test_thread_count		{ 16 };
constexpr size_t total_test_count		{ 320'000'00 };
size_t system_thread_count				{ 1 };

ALock				a_lock(test_thread_count);
TTASLock			ttas_lock;
TASLock				tas_lock;
CLHLock				clh_lock;
MCSLock				mcs_lock;
mutex				mtx;
CRITICAL_SECTION	critical_section;
SRWLOCK				rw_lock;
BackoffLock			backoff_lock;
ReadWriteLock		read_write_lock;
FastSpinLock		fs_lock;

size_t g_counter {};

void initialize();

void test_mutex(size_t test_count);
void test_critical_section(size_t test_count);
void test_rw_lock(size_t test_count);
void test_alock(size_t test_count);
void test_ttaslock(size_t test_count);
void test_taslock( size_t test_count);
void test_clhlock( size_t test_count);
void test_mcslock(size_t test_count);
void test_backofflock(size_t test_count);
void test_readwritelock(size_t test_count);
void test_fastspinlock(size_t test_count);
void test_procedure_base(void(*lock_procedure)(size_t), const std::string_view& str_v, size_t thread_count, size_t test_count);

struct testing_context
{
	void (*lock_procedure)(size_t);
	std::string_view text;
};


auto main() -> void
{
	std::cout << __FILE__ << std::endl;

	initialize();

	std::vector<testing_context> tests;

	//tests.push_back({ test_taslock,				"tas_lock" });
	//tests.push_back({ test_ttaslock,			"ttas_lock" });
	//tests.push_back({ test_backofflock,			"backofflock" });
	//tests.push_back({ test_alock,				"alock" });
	//tests.push_back({ test_clhlock,				"clhlock" });
	//tests.push_back({ test_mcslock,				"mcslock" });

	tests.push_back({ test_fastspinlock,		"fastspinlock" });
	//tests.push_back({ test_readwritelock,		"custom_readwrtielock" });
	//tests.push_back({ test_mutex,				"std::mutex" });
	//tests.push_back({ test_rw_lock,				"rw_lock" });
	//tests.push_back({ test_critical_section,	"critical_section" });


	for (int thread_count{ 2 }; thread_count <= system_thread_count; thread_count *= 2)
	{
		std::cout << "--------------------- thread count : " << thread_count << " --------------------" << endl;
		std::cout << "| lock type";
		std::cout.width( 31  );
		std::cout << "| counter ";
		std::cout.width(6);
		std::cout << "| delta tick	 |\n";


		for (const auto& test : tests)
		{
			test_procedure_base(test.lock_procedure, test.text, thread_count, total_test_count / thread_count );
		}

		std::cout << "---------------------------------------------------------------------------" << endl;

	}
	
	int n;
	cin >> n;

	return; 
}



void initialize()
{
	InitializeCriticalSection(&critical_section);
	InitializeSRWLock(&rw_lock);

	system_thread_count	= std::thread::hardware_concurrency();
	cout << "system therad count : "<< system_thread_count << endl;
}

void test_mutex(size_t test_count)
{
	for (int n{}; n < test_count; ++n)
	{
		mtx.lock();

		g_counter += 1;

		mtx.unlock();
	}
}

void test_critical_section(size_t test_count)
{
	for (int n{}; n < test_count; ++n)
	{
		EnterCriticalSection(&critical_section);

		g_counter += 1;

		LeaveCriticalSection(&critical_section);
	}
}

void test_rw_lock(size_t test_count)
{
	for (int n{}; n < test_count; ++n)
	{
		AcquireSRWLockExclusive(&rw_lock);

		g_counter += 1;

		ReleaseSRWLockExclusive(&rw_lock);
	}
}


void test_alock(size_t test_count)
{
	for (int n{}; n < test_count; ++n)
	{
		a_lock.lock();

		g_counter += 1;

		a_lock.unlock();
	}
}

void test_ttaslock(size_t test_count)
{
	for (int n{}; n < test_count; ++n)
	{
		ttas_lock.lock();

		g_counter += 1;

		ttas_lock.unlock();
	}
}

void test_taslock(size_t test_count)
{
	for (int n{}; n < test_count; ++n)
	{
		tas_lock.lock();

		g_counter += 1;

		tas_lock.unlock();
	}
}


void test_clhlock(size_t test_count)
{
	for (int n{}; n < test_count; ++n)
	{
		clh_lock.lock();

		g_counter += 1;

		clh_lock.unlock();
	}
}


void test_mcslock(size_t test_count)
{
	for (int n{}; n < test_count; ++n)
	{
		mcs_lock.lock();

		g_counter += 1;

		mcs_lock.unlock();
	}
}

void test_backofflock(size_t test_count)
{
	for (int n{}; n < test_count; ++n)
	{
		backoff_lock.lock();

		g_counter += 1;

		backoff_lock.unlock();
	}
}



void test_readwritelock(size_t test_count)
{
	for (int n{}; n < test_count; ++n)
	{
		read_write_lock.lock();

		g_counter += 1;

		read_write_lock.unlock();
	}
}

void test_fastspinlock(size_t test_count)
{
	for (int n{}; n < test_count; ++n)
	{
		fs_lock.EnterLock();

		g_counter += 1;

		fs_lock.LeaveLock();
	}
}

void test_procedure_base(void(*lock_procedure)(size_t), const string_view& str_v
						 , size_t thread_count, size_t test_count)
{
	std::vector<std::thread> ths;
	ths.reserve(thread_count);

	size_t start_tick = GetTickCount64();

	for (int n{}; n < thread_count; ++n)
	{
		ths.emplace_back(lock_procedure, test_count);
	}

	for (auto& th : ths)
	{
		th.join();
	}

	size_t delta_tick = GetTickCount64() - start_tick;

	std::cout << str_v;
	std::cout.width(30 - str_v.length());
	cout << "	| " << g_counter << "| " << delta_tick << "	|" << endl;

	g_counter = 0;
}
