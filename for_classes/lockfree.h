#pragma once
#include <atomic>

using namespace std;

bool cas(atomic_int* addr, int expected, int new_value)
{
	return atomic_compare_exchange_strong(addr, &expected, new_value);
}

bool cas(volatile int* addr, int expected, int new_val) 
{ 
	return atomic_compare_exchange_strong(reinterpret_cast<volatile atomic_int*>(addr), &expected, new_val); 
}

