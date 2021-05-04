#pragma once

#include <type_traits>


template<typename T = int>
T fast_rand();


template<typename T = int>
T fast_rand()
{
	static_assert(std::is_integral<T>::value, "");

	alignas(64) static int seed = 'fast';

	seed = (214013 * seed + 2531011);

	return (T)((seed >> 16) & 0x7FFF);
}




