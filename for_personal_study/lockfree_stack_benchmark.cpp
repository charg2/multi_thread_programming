//#include <iostream>
//#include <thread>
//#include <vector>
//#include <mutex>
//#include <chrono>
//#include <string_view>
//
//#include "ConcurrentStack.h"
//
//using namespace std;
//
//constexpr size_t total_test_count{ 1000'0000 };
//size_t system_thread_count{ 1 };
//int64_t g_counter{};
//
//c2::concurrency::ConcurrentStack<int>		concurrent_stack64;
//c2::concurrency::ConcurrentStack128<int>	concurrent_stack128;
//
//void initialize();
//void push_concurrent_stack128(size_t test_count);
//void push_concurrent_stack64( size_t test_count);
//void pop_concurrent_stack128( size_t test_count);
//void pop_concurrent_stack64(  size_t test_count);
//
//
//
//void test_procedure_base(void(*lock_procedure)(size_t), const std::string_view& str_v, size_t thread_count, size_t test_count);
//
//struct task_context
//{
//	void (*lock_procedure)(size_t);
//
//	std::string_view	text;
//};
//
//
//auto main() -> void
//{
//	std::cout << __FILE__ << std::endl;
//
//	initialize();
//
//	std::vector<task_context> tasks;
//
//	tasks.push_back({ push_concurrent_stack128,	"push_concurrent_stack128" });
//	tasks.push_back({ push_concurrent_stack64 ,	"push_concurrent_stack64 " });
//	tasks.push_back({ pop_concurrent_stack128 ,	"pop_concurrent_stack128 " });
//	tasks.push_back({ pop_concurrent_stack64  ,	"pop_concurrent_stack64  " });
//
//	int thread_count{ 1 };
//	for (; thread_count <= system_thread_count; thread_count *= 2)
//	{
//		std::cout << "--------------------- thread count : " << thread_count << " --------------------" << endl;
//		std::cout << "		type";
//		std::cout.width(31);
//		std::cout << "| counter ";
//		std::cout.width(6);
//		std::cout << "| delta tick	 |\n";
//
//		for (const auto& test : tasks)
//		{
//			test_procedure_base(test.lock_procedure, test.text, thread_count, total_test_count / thread_count);
//		}
//
//		std::cout << "---------------------------------------------------------------------------" << endl;
//	}
//
//
//	int n;
//	cin >> n;
//
//	return;
//}
//
//
//
//void initialize()
//{
//	system_thread_count = std::thread::hardware_concurrency();
//	cout << "system therad count : " << system_thread_count << endl;
//}
//
//
//
//void test_procedure_base(void(*lock_procedure)(size_t), const string_view& str_v, size_t thread_count, size_t test_count)
//{
//	std::vector<std::thread> ths;
//	ths.reserve(thread_count);
//
//	auto start_time = chrono::high_resolution_clock::now();
//
//	for (int n{1}; n <= thread_count; ++n)
//	{
//		ths.emplace_back(lock_procedure, test_count);
//	}
//
//	for (auto& th : ths)
//	{
//		th.join();
//	}
//
//	auto delta_time = chrono::high_resolution_clock::now() - start_time;
//
//
//	std::cout << str_v;
//	std::cout.width(30 - str_v.length());
//	cout << "	| " << g_counter << "| " << 
//		chrono::duration_cast<chrono::milliseconds>(delta_time).count() << "	|" << endl;
//
//	g_counter = 0;
//}
//
//void push_concurrent_stack128(size_t test_count)
//{
//	for (size_t n{}; n < test_count; ++n)
//	{ 
//		concurrent_stack128.push(n);
//	}
//}
//
//void push_concurrent_stack64(size_t test_count)
//{
//	for (size_t n{}; n < test_count; ++n)
//	{
//		concurrent_stack64.push(n);
//	}
//}
//
//void pop_concurrent_stack128(size_t test_count)
//{
//	int64_t local_counter{};
//
//	int p{};
//
//	while (concurrent_stack128.try_pop(p))
//	{
//		local_counter += 1;
//	}
//
//	InterlockedAdd64((volatile int64_t*)&g_counter, local_counter);
//}
//
//void pop_concurrent_stack64(size_t test_count)
//{
//	int64_t local_counter{};
//
//	int p{};
//	while (concurrent_stack64.try_pop(p))
//	{
//		local_counter += 1;
//	}
//
//	InterlockedAdd64((volatile int64_t*)&g_counter, local_counter);
//}
