#pragma once
#include <atomic>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <set>


#include "unbounded_queue.h"
#include "lockfree_unbounded_queue.h"


using namespace std;
using namespace std::chrono;

//constexpr size_t test_case = 1000'000;
constexpr size_t test_case = 10'000'000;
constexpr size_t key_range = 100;
constexpr size_t MAX_THREAD = 8;

unbounded_queue my_queue;
//lockfree_unbounded_queue my_queue;

void do_benchmark(int num_thread)
{
	for (int i = 0; i < test_case / num_thread; ++i)
	{
		if ((rand() % 2) || (i < 10000 / num_thread))
		{
			my_queue.enqueue(i);
			//cout << i << " ";
		}
		else
		{
			my_queue.dequeue();
		}
	}
}


int main()
{
	my_queue.unsafe_clear();

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
		my_queue.unsafe_display20();
		my_queue.unsafe_clear();
	}

	int i;
	cin >> i;
	return 0;
}

/*
Number of Threads = 1, Exec Time = 3646ms,
9979726 , 9979727 , 9979728 , 9979731 , 9979732 , 9979734 , 9979736 , 9979738 , 9979741 , 9979742 , 9979743 , 9979744 , 9979745 , 9979746 , 9979748 , 9979750 , 9979751 , 9979752 , 9979754 , 9979756 ,
Number of Threads = 2, Exec Time = 3219ms,
4979617 , 4979618 , 4979621 , 4979624 , 4979625 , 4979626 , 4979627 , 4979628 , 4979629 , 4979630 , 4979631 , 4979632 , 4979634 , 4979635 , 4979636 , 4979638 , 4979639 , 4979642 , 4979643 , 4979645 ,
Number of Threads = 4, Exec Time = 2831ms,
2479725 , 2479727 , 2479732 , 2479733 , 2479734 , 2479735 , 2479738 , 2479744 , 2479747 , 2479750 , 2479751 , 2479753 , 2479755 , 2479757 , 2479758 , 2479768 , 2479771 , 2479772 , 2479773 , 2479776 ,
Number of Threads = 8, Exec Time = 3460ms,
1235170 , 1237055 , 1235171 , 1235172 , 1235173 , 1237058 , 1235174 , 1237059 , 1235176 , 1237062 , 1235179 , 1237064 , 1237066 , 1235182 , 1237067 , 1235183 , 1237069 , 1237071 , 1237072 , 1235188 ,

lock free 
Number of Threads = 1, Exec Time = 4208ms,
9979726 , 9979727 , 9979728 , 9979731 , 9979732 , 9979734 , 9979736 , 9979738 , 9979741 , 9979742 , 9979743 , 9979744 , 9979745 , 9979746 , 9979748 , 9979750 , 9979751 , 9979752 , 9979754 , 9979756 ,
Number of Threads = 2, Exec Time = 3387ms,
4979617 , 4979618 , 4979621 , 4979624 , 4979625 , 4979626 , 4979627 , 4979628 , 4979629 , 4979630 , 4979631 , 4979632 , 4979634 , 4979635 , 4979636 , 4979638 , 4979639 , 4979642 , 4979643 , 4979645 ,
Number of Threads = 4, Exec Time = 2925ms,
2479725 , 2479727 , 2479732 , 2479733 , 2479734 , 2479735 , 2479738 , 2479744 , 2479747 , 2479750 , 2479751 , 2479753 , 2479755 , 2479757 , 2479758 , 2479768 , 2479771 , 2479772 , 2479773 , 2479776 ,
Number of Threads = 8, Exec Time = 3541ms,
1233634 , 1238723 , 1233635 , 1238725 , 1238726 , 1233639 , 1238727 , 1233640 , 1238731 , 1233644 , 1238734 , 1238736 , 1238737 , 1233652 , 1233653 , 1238740 , 1238741 , 1233656 , 1238743 , 1238744 ,

*/
