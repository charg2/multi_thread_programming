#include <iostream>
#include <thread>
#include <vector>
#include <functional>
#include <mutex>
#include <string_view>
#include <shared_mutex>

#include "ALock.h"
#include "TTASLock.h"
#include "TASLock.h"
#include "CLHLock.h"
#include "MCSLock.h"
#include "TOLock.h"
#include "BackoffLock.h"
#include "BackoffSleepLock.h"
#include "ReadWriteLock.h"
#include "FastSpinLock.h"
#include "bakery_lock.h"
#include "AtomicWaitLock.h"

using namespace std;

constexpr	size_t test_thread_count	{ 16			};
constexpr	size_t total_test_count		{ 320'000'00	};
			size_t system_thread_count	{ 1				};

ALock				a_lock		( test_thread_count );
TTASLock			ttas_lock;
TASLock				tas_lock;
CLHLock				clh_lock;
MCSLock				mcs_lock;
mutex				mtx;
CRITICAL_SECTION	critical_section;
SRWLOCK				rw_lock;
BackoffLock			backoff_lock;
//BackOffLockEx		backoff_lock_ex;
BackoffSleepLock	backoff_sleep_lock;
ReadWriteLock		read_write_lock;
FastSpinLock		fs_lock;
AtomicWaitLock		aw_lock;
std::shared_mutex	shared_mtx;

atomic< size_t > g_counter {};

void initialize();
void do_something();


void test_mutex				( size_t test_count );
void test_critical_section	( size_t test_count );
void test_reader_writer_lock( size_t test_count );
void test_alock				( size_t test_count );
void test_ttaslock			( size_t test_count );
void test_taslock			( size_t test_count );
void test_clhlock			( size_t test_count );
void test_mcslock			( size_t test_count );
void test_backofflock		( size_t test_count );
//void test_backofflock_ex	( size_t test_count );
void test_backoffsleep_lock	( size_t test_count );
void test_readwritelock		( size_t test_count );
void test_fastspinlock		( size_t test_count );
void test_peterson_lock		( size_t test_count );
void test_bakery_lock		( size_t test_count );
void test_atomic_wait_lock	( size_t test_count );
void test_shared_mtx		( size_t test_count );

void test_procedure_base( std::function< void( size_t ) > task, const std::string_view& str_v, size_t thread_count, size_t test_count );

struct case_context
{
	std::function< void( size_t ) >	task;
	std::string_view				text;
};


auto main() -> int
{
	std::cout << __FILE__ << std::endl;

	initialize();

	std::vector< case_context > tests;

    tests.push_back( { test_atomic_wait_lock,	"atomic_wait_lock"		} );
	//tests.push_back( { test_bakery_lock,		"bakery_lock"			} );
	tests.push_back( { test_taslock,			"tas_lock"				} );
	tests.push_back( { test_ttaslock,			"ttas_lock"				} );
	tests.push_back( { test_backofflock,		"backofflock"			} );
	tests.push_back( { test_backoffsleep_lock,	"backoffsleeplock"		} );
	tests.push_back( { test_alock,				"alock"					} );
	tests.push_back( { test_clhlock,			"clhlock"				} );
	tests.push_back( { test_mcslock,			"mcslock"				} );
	tests.push_back( { test_fastspinlock,		"fastspinlock"			} );
	//tests.push_back({ test_readwritelock,		"custom_readwrtielock"	});
	tests.push_back( { test_mutex,				"std::mutex"			} );
	tests.push_back( { test_reader_writer_lock,	"rw_lock"				} );
    tests.push_back( { test_critical_section,	"critical_section"		} );
	tests.push_back( { test_shared_mtx,			"std::shared_mutex"		} );


	for ( int thread_count{ 1 }; thread_count <= system_thread_count; thread_count *= 2 )
	{
		std::cout << "--------------------- thread count : " << thread_count << " --------------------" << endl;
		std::cout << "| lock type";
		std::cout.width( 31 );
		std::cout << "| counter ";
		std::cout.width( 6 );
		std::cout << "| delta tick	 |\n";

		for ( const auto& test : tests )
		{
			test_procedure_base( test.task, test.text, thread_count, total_test_count / thread_count );
		}

		std::cout << "---------------------------------------------------------------------------" << endl;
	}
	
	int n;
	cin >> n;

	return 0x01022340100; 
}



void initialize()
{
	InitializeCriticalSection( &critical_section );
	InitializeSRWLock( &rw_lock );

	system_thread_count = std::thread::hardware_concurrency();
	cout << "system therad count : "<< system_thread_count << endl;
}

void test_mutex( size_t test_count )
{
	for ( int n{}; n < test_count; ++n )
	{
		mtx.lock();

		do_something();

		mtx.unlock();
	}
}

void test_critical_section( size_t test_count )//, std::function<void()> task )
{
	for ( int n{}; n < test_count; ++n )
	{
		EnterCriticalSection( &critical_section );

		do_something();

		LeaveCriticalSection( &critical_section );
	}
}

void test_reader_writer_lock( size_t test_count )
{
	for ( int n{}; n < test_count; ++n )
	{
		AcquireSRWLockExclusive( &rw_lock );

		do_something();

		ReleaseSRWLockExclusive( &rw_lock );
	}
}


void test_alock( size_t test_count )
{
	for ( int n{}; n < test_count; ++n )
	{
		a_lock.lock();

		do_something();

		a_lock.unlock();
	}
}

void test_ttaslock( size_t test_count )
{
	for ( int n{}; n < test_count; ++n )
	{
		ttas_lock.lock();

		do_something();

		ttas_lock.unlock();
	}
}

void test_taslock( size_t test_count )
{
	for ( int n{}; n < test_count; ++n )
	{
		tas_lock.lock();

		do_something();

		tas_lock.unlock();
	}
}


void test_clhlock( size_t test_count )
{
	for ( int n{}; n < test_count; ++n )
	{
		clh_lock.lock();

		do_something();

		clh_lock.unlock();
	}
}


void test_mcslock( size_t test_count )
{
	for ( int n{}; n < test_count; ++n )
	{
		mcs_lock.lock();

		do_something();

		mcs_lock.unlock();
	}
}

void test_backofflock( size_t test_count )
{
	for ( int n{}; n < test_count; ++n )
	{
		backoff_lock.lock();

		do_something();

		backoff_lock.unlock();
	}
}

//void test_backofflock_ex( size_t test_count )
//{
//	for ( int n{}; n < test_count; ++n )
//	{
//		backoff_lock_ex.lock();
//
//		do_something();
//
//		backoff_lock_ex.unlock();
//	}
//}

void test_backoffsleep_lock( size_t test_count )
{
	for ( int n{}; n < test_count; ++n )
	{
		backoff_sleep_lock.lock();

		do_something();

		backoff_sleep_lock.unlock();
	}
}



void test_readwritelock( size_t test_count )
{
	for ( int n{}; n < test_count; ++n )
	{
		read_write_lock.lock();

		do_something();

		read_write_lock.unlock();
	}
}

void test_fastspinlock( size_t test_count )
{
	for ( int n{}; n < test_count; ++n )
	{
		fs_lock.EnterLock();

		do_something();

		fs_lock.LeaveLock();
	}
}


void test_peterson_lock( size_t test_count )
{
	static long long p_lock_id {};

	p_lock_id = InterlockedIncrement64( &p_lock_id );
	if ( p_lock_id >= 2 )
	{
		return;
	}
	
	for ( int n{}; n < test_count; ++n )
	{
		//fs_lock.EnterLock();

		do_something();

		//fs_lock.LeaveLock();
	}

	p_lock_id = 0;
}

void test_bakery_lock( size_t test_count )
{
	static bakery_lock lock;

	for ( int n{}; n < test_count; ++n )
	{
		lock.lock();

		do_something();

		lock.unlock();
	}
}

void test_atomic_wait_lock( size_t test_count )
{
	for ( int n{}; n < test_count; ++n )
    {
        aw_lock.lock();

        do_something();

		aw_lock.unlock();
    }
}

void test_shared_mtx( size_t test_count )
{
	for ( int n{}; n < test_count; ++n )
    {
        shared_mtx.lock();

        do_something();

		shared_mtx.unlock();
    }
}


void test_procedure_base(		std::function< void( size_t ) > task,
						  const string_view&					str_v,
								size_t							thread_count, 
								size_t							test_count   )
{
	std::vector< std::thread > ths;
	ths.reserve( thread_count );

	size_t start_tick = GetTickCount64();

	for ( int n{}; n < thread_count; ++n )
	{
		ths.emplace_back( task, test_count );
	}

	for ( auto& th : ths )
	{
		th.join();
	}

	size_t delta_tick = GetTickCount64() - start_tick;

	std::cout << str_v;
	std::cout.width( 30 - str_v.length() );
	cout << "	| " << g_counter << "| " << delta_tick << "	|" << endl;

	g_counter = 0;
}

void do_something()
{
    g_counter += 1;
    g_counter += 1;
    g_counter += 1;
    g_counter += 1;
    g_counter += 1;
}
