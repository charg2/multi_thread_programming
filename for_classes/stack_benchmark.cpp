#pragma once
#include <atomic>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <set>


//#include "unbounded_queue.h"
#include "lockfree_stack.h"


using namespace std;
using namespace std::chrono;

//constexpr size_t test_case = 1000'000;
constexpr size_t test_case = 1000'0000;
constexpr size_t key_range = 100;
constexpr size_t max_thread = 8;

//lockfree_stack my_stack;
//coarse_grain_stack my_stack;
lockfree_backoff_stack my_stack;

void do_benchmark(int num_thread)
{
	for (int i = 0; i < test_case / num_thread; ++i)
	{
		if ((rand() % 2 == 0) || (i < ( 2/ num_thread)))
		{
			my_stack.push(i);
		}
		else
		{
			my_stack.pop();
		}
	}


}


int main()
{
	my_stack.unsafe_clear();

	for (size_t thread_count = 1; thread_count <= max_thread; thread_count *= 2)
	{
		vector<thread> threads;

		auto start_time = high_resolution_clock::now();

		for (int key = 0; key < thread_count; ++key)
		{
			threads.emplace_back(do_benchmark, thread_count);
		}


		for (auto& t : threads)
		{
			t.join();
		}

		auto end_time = high_resolution_clock::now();
		auto exec_time = end_time - start_time;
		cout << "number of threads = " << thread_count << ", ";
		cout << "exec time = " << duration_cast<milliseconds>(exec_time).count() << "ms, \n";


		my_stack.unsafe_display20();
		my_stack.unsafe_clear();
	}

	int i;
	cin >> i;
	return 0;
}


/*
* 
* 
* coarse grain 
* number of threads = 1, exec time = 1250ms,
9932503, 9932502, 9932481, 9932458, 9932457, 9932456, 9932455, 9929546, 9929507, 9929506, 9929505, 9929504, 9929493, 9929360, 9929359, 9929358, 9929357, 9929356, 9929355, 9929332,
number of threads = 2, exec time = 1290ms,
4999983, 4999982, 4999981, 4999976, 4999971, 4999970, 4999965, 4999964, 4999899, 4999892, 4999857, 4967556, 4967555, 4967456, 4967451, 4967450, 4967449, 4967446, 4967439, 4967438,
number of threads = 4, exec time = 1710ms,
2499999, 2499970, 2499939, 2499938, 2499937, 2499936, 2499901, 2499898, 2499897, 2499826, 2499825, 2499824, 2499823, 2485348, 2485347, 2485346, 2485343, 2485340, 2485339, 2485338,
number of threads = 8, exec time = 2799ms,
1249999, 1249998, 1247275, 1247272, 1247271, 1247246, 1247239, 1247238, 1247237, 1247166, 1247165, 1247164, 1247161, 1247160, 1247139, 1247138, 1247137, 1247132, 1247127, 1247120,
* 

* 
lockfree stack 

number of threads = 1, exec time = 989ms,
9932503, 9932502, 9932481, 9932458, 9932457, 9932456, 9932455, 9929546, 9929507, 9929506, 9929505, 9929504, 9929493, 9929360, 9929359, 9929358, 9929357, 9929356, 9929355, 9929332,
number of threads = 2, exec time = 868ms,
4999983, 4999982, 4999981, 4999976, 4999971, 4999970, 4999965, 4999964, 4999899, 4999892, 4999857, 4967556, 4967555, 4967456, 4967451, 4967450, 4967449, 4967446, 4967439, 4967438,
number of threads = 4, exec time = 1881ms,
2499999, 2499970, 2499939, 2499938, 2499937, 2499936, 2499901, 2499898, 2499897, 2499826, 2499825, 2499824, 2499823, 2485225, 2485200, 2485199, 2492878, 2485194, 2492875, 2492874,
number of threads = 8, exec time = 2248ms,
1249999, 1249998, 1247275, 1247272, 1247271, 1247246, 1247239, 1247238, 1247237   , 1247166, 1247165, 1247164, 1247161, 1247160, 1247139, 1247138, 1247137, 1247132, 1247127, 1247120,


lockfree backoff stack

number of threads = 1, exec time = 799ms,
9932503, 9932502, 9932481, 9932458, 9932457, 9932456, 9932455, 9929546, 9929507, 9929506, 9929505, 9929504, 9929493, 9929360, 9929359, 9929358, 9929357, 9929356, 9929355, 9929332,
number of threads = 2, exec time = 937ms,
4999977, 4999976, 4999975, 4999974, 4999861, 4999856, 4999855, 4999728, 4999727, 4999720, 4999657, 4999654, 4999653, 4962500, 4962499, 4962498, 4962491, 4962488, 4962487, 4962394,
number of threads = 4, exec time = 2297ms,
2499999, 2499998, 2499997, 2499722, 2499721, 2499716, 2499715, 2499714, 2498723, 2498712, 2497451, 2497450, 2497449, 2497422, 2497415, 2497412, 2497411, 2497408, 2497407, 2497406,
number of threads = 8, exec time = 3279ms,
1249999, 1249998, 1249995, 1249994, 1249979, 1249978, 1249975, 1249968, 1249967, 1249958, 1249957, 1249956, 1249955, 1249802, 1249799, 1249798, 1249789, 1249784, 1249783, 1248566,

*/
