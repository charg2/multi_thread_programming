#pragma once
#include <atomic>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <set>


#include "unbounded_queue.h"


using namespace std;
using namespace std::chrono;

//constexpr size_t test_case = 1000'000;
constexpr size_t test_case = 10'000'000;
constexpr size_t key_range = 100;
constexpr size_t MAX_THREAD = 8;

CUQueue my_queue;

void do_benchmark(int num_thread)
{
	for (int i = 0; i < test_case / num_thread; ++i)
	{
		if ( (rand() % 2) || (i < 10000 / num_thread))
		{
			my_queue.enq(i);
			//cout << i << " ";
		}
		else
		{
			my_queue.deq();
		}
	}
}


int main()
{
	my_queue.clear();

	for (size_t thread_count = 1; thread_count <= 8; thread_count *= 2)
	{
		vector<thread> threads;

		auto start_time = high_resolution_clock::now();

		for (int n = 0; n < thread_count; ++n)
		{
			threads.emplace_back(do_benchmark, thread_count);
		}


		for (auto& t : threads)
		{
			t.join();
		}

		auto end_time = high_resolution_clock::now();
		auto exec_time = end_time - start_time;
		cout << "Number of Threads = " << thread_count << ", ";
		cout << "Exec Time = " << duration_cast<milliseconds>(exec_time).count() << "ms, \n";
		my_queue.display20();
		my_queue.clear();
	}

	int i;
	cin >> i;
	return 0;
}
