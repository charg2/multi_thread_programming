#pragma once

#include <cstdint>


namespace threeadlocal
{
	int32_t get_thread_id();
	inline thread_local int32_t thread_id{ -1 };
}