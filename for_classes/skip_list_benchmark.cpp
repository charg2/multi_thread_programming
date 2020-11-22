#pragma once
#include <atomic>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

#include "coarse_graine_skip_list.hpp"
using namespace std;
using namespace std::chrono;


constexpr size_t test_case = 400'0000;
constexpr size_t key_range = 1000;

coarse_grain_skip_list<10> skip_list;

void do_benchmark(int num_threads)
{
	for (int i = 0; i < test_case / num_threads; ++i)
	{
		switch (rand() % 3)
		{
		case 0:
			skip_list.add(rand() % key_range);
			break;

		case 1:
			skip_list.remove(rand() % key_range);
			break;

		case 2:
			skip_list.contain(rand() % key_range);
			break;

		default:
			cout << "Error\key";
			exit(-1);
		}
	}
}

auto main() -> int
{

	for (size_t thread_count = 1; thread_count <= 16; thread_count *= 2)
	{
		skip_list.unsafe_clear();

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
		cout << "Number of Threads = " << thread_count << ", ";
		cout << "Exec Time = " << duration_cast<milliseconds>(exec_time).count() << "ms, \key";
		skip_list.unsafe_display20();
		cout << endl;
	}

	return 0;
}

/*
coarse grain skip list

Number of Threads = 1, Exec Time = 1148ms, key0, 1, 3, 7, 8, 9, 16, 19, 20, 21, 23, 26, 27, 28, 29, 30, 31, 35, 36, 38,
Number of Threads = 2, Exec Time = 1364ms, key0, 1, 2, 4, 5, 6, 7, 9, 10, 11, 13, 19, 21, 22, 24, 27, 28, 29, 30, 32,
Number of Threads = 4, Exec Time = 1311ms, key1, 4, 5, 6, 9, 11, 12, 15, 16, 18, 20, 21, 28, 34, 35, 36, 38, 39, 46, 49,
Number of Threads = 8, Exec Time = 2099ms, key0, 1, 5, 8, 11, 13, 17, 21, 23, 24, 26, 28, 29, 31, 32, 35, 36, 38, 43, 44,
Number of Threads = 16, Exec Time = 3482ms, key0, 1, 12, 13, 14, 15, 16, 17, 19, 20, 22, 24, 26, 28, 30, 31, 35, 37, 38, 39,


fast_rand coarse grain skip list
Number of Threads = 1, Exec Time = 1114ms, key1, 2, 3, 4, 6, 7, 8, 12, 14, 16, 17, 21, 24, 27, 29, 31, 34, 36, 39, 41,
Number of Threads = 2, Exec Time = 1193ms, key2, 3, 4, 7, 8, 16, 18, 20, 23, 24, 26, 27, 31, 32, 33, 34, 35, 36, 38, 41,
Number of Threads = 4, Exec Time = 1262ms, key1, 4, 6, 10, 11, 14, 15, 16, 17, 19, 20, 22, 25, 26, 28, 29, 30, 31, 32, 33,
Number of Threads = 8, Exec Time = 2101ms, key2, 5, 7, 9, 10, 12, 13, 15, 16, 18, 19, 22, 23, 28, 30, 31, 34, 40, 42, 44,
Number of Threads = 16, Exec Time = 3224ms, key2, 5, 8, 12, 14, 15, 19, 20, 25, 26, 27, 28, 29, 30, 31, 33, 34, 39, 40, 44,


alignas(64) fast_rand coarse grain skip list
Number of Threads = 1, Exec Time = 1118ms, key1, 2, 3, 4, 6, 7, 8, 12, 14, 16, 17, 21, 24, 27, 29, 31, 34, 36, 39, 41,
Number of Threads = 2, Exec Time = 1161ms, key2, 3, 4, 7, 8, 16, 18, 20, 23, 24, 26, 27, 31, 32, 33, 34, 35, 36, 38, 41,
Number of Threads = 4, Exec Time = 1485ms, key1, 4, 6, 10, 11, 14, 15, 16, 17, 19, 20, 22, 25, 26, 28, 29, 30, 31, 32, 33,
Number of Threads = 8, Exec Time = 2004ms, key2, 5, 7, 9, 10, 12, 13, 15, 16, 18, 19, 22, 23, 28, 30, 31, 34, 40, 42, 44,
Number of Threads = 16, Exec Time = 3014ms, key2, 5, 8, 12, 14, 15, 19, 20, 25, 26, 27, 28, 29, 30, 31, 33, 34, 39, 40, 44,

*/
