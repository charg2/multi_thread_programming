#pragma once
#include <atomic>

using namespace std;

bool cas(atomic<size_t>* addr, size_t expected, size_t new_value)
{
	return atomic_compare_exchange_strong(addr, &expected, new_value);
}

bool cas(volatile size_t* addr, size_t expected, size_t new_val) 
{ 
	return atomic_compare_exchange_strong(reinterpret_cast<volatile atomic<size_t>*>(addr), &expected, new_val); 
}


bool cas32(atomic<int>* addr, int expected, int new_value)
{
	return atomic_compare_exchange_strong(addr, &expected, new_value);
}

bool cas32(volatile int* addr, int expected, int new_val)
{
	return atomic_compare_exchange_strong(reinterpret_cast<volatile atomic<int>*>(addr), &expected, new_val);
}

bool cas64(atomic<long long>* addr, long long expected, long long new_value)
{
	return atomic_compare_exchange_strong(addr, &expected, new_value);
}

bool cas64(volatile long long* addr, long long expected, long long new_val)
{
	return atomic_compare_exchange_strong(reinterpret_cast<volatile atomic<long long>*>(addr), &expected, new_val);
}

