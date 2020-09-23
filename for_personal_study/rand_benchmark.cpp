//#include "backoff.h"
//#include "fastrand.h"
//#include <random>
//#include <chrono>
//#include <iostream>
//
//using namespace std;
//using namespace chrono;
//
//constexpr size_t test_case{ 100'000'000 } ;
//
//
//void main()
//{
//	{
//	auto start_time = chrono::high_resolution_clock::now();
//	for (int n{}; n < test_case; ++n)
//	{
//		fast_rand();
//	}
//
//	auto delta_time = chrono::high_resolution_clock::now() - start_time;
//
//	cout <<
//		chrono::duration_cast<chrono::milliseconds>(delta_time).count() << "	|" << endl;
//
//	}
//
//	{
//
//		auto start_time = chrono::high_resolution_clock::now();
//		for (int n{}; n < test_case; ++n)
//		{
//			rand();
//		}
//
//		auto delta_time = chrono::high_resolution_clock::now() - start_time;
//
//		cout <<
//			chrono::duration_cast<chrono::milliseconds>(delta_time).count() << "	|" << endl;
//
//	}
//
//	BackOff backoff{1000};
//	backoff.do_backoff();
//}
//
//
//
