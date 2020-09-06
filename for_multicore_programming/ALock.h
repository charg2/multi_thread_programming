#pragma once
#include <Windows.h>
#include <cstdint>


// array based lock
class ALock
{
public:
	ALock(size_t capacity) : size{ capacity }
	{
		flags = new int256_t[capacity];
		tail = 0;
		flags[0].value = 1;

		tls_index = TlsAlloc();
		if (tls_index == TLS_OUT_OF_INDEXES)
		{
			int* hazard_ptr{}; 
			*hazard_ptr = 0;
		}
	}

	void lock()
	{
		int16_t slot = (InterlockedIncrement16(&tail)) % size;
		TlsSetValue(tls_index, (void*)slot);
		for (; flags[slot].value == 0 ;)
		{}
	}

	void unlock()
	{
		int16_t slot = reinterpret_cast<int16_t>(TlsGetValue(tls_index));
		
		flags[slot].value = 0;
		flags[(slot + 1) % size].value = 1;
	}

private:
	static constexpr size_t cache_line_size{ 64 };
	struct int256_t
	{
		alignas(cache_line_size) int value;
	};

private:
	int256_t*	flags;
	int32_t		tls_index;
	int16_t		tail;
	int16_t		size;
};



//#pragma once
//#include <Windows.h>
//#include <atomic>
//#include <vector>
//#include <cstdint>
//
//
//// array based lock
//class ALock
//{
//public:
//	ALock(size_t capacity)
//	{
//		flags.resize(capacity);
//		flags[0].value = 1;
//		
//		tail = 0;
//		
//		tls_index = TlsAlloc();
//		if (tls_index == TLS_OUT_OF_INDEXES)
//		{
//			int* hazard_ptr{};
//			*hazard_ptr = 0;
//		}
//	}
//
//	void lock()
//	{
//		int16_t slot = (++tail) % flags.size();
//		TlsSetValue(tls_index, (void*)slot);
//		for (; flags[slot].value == 0;)
//		{
//		}
//	}
//
//	void unlock()
//	{
//		int16_t slot = reinterpret_cast<uint32_t>(TlsGetValue(tls_index));
//
//		flags[slot].value = 0;
//		flags[(slot + 1) % flags.size()].value = 1;
//	}
//
//private:
//	static constexpr size_t cache_line_size{ 64 };
//	struct int256_t
//	{
//		alignas(cache_line_size) int value;
//	};
//
//private:
//	std::vector<int256_t>	flags;
//	std::atomic<int16_t>	tail;
//	int32_t					tls_index;
//};
//
