#pragma once
#include <Windows.h>
#include <cstdint>


// array based lock
class ALock
{
public:
	ALock(int64_t capacity) : size{ capacity }, tls_index{ TLS_OUT_OF_INDEXES }
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

	~ALock()
	{
		delete[] flags;

		if (tls_index != TLS_OUT_OF_INDEXES)
		{
			TlsFree(tls_index);
		}
	}


	void lock()
	{
		int64_t slot = (InterlockedIncrement64(&tail)) % size;
		TlsSetValue(tls_index, (void*)slot);
		for (; flags[slot].value == 0 ;)
		{}
	}

	void unlock()
	{
		int64_t slot = reinterpret_cast<int64_t>(TlsGetValue(tls_index));
		
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
	int64_t		tail;
	int64_t		size;
	uint32_t	tls_index;
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
//		int64_t slot = (++tail) % flags.size();
//		TlsSetValue(tls_index, (void*)slot);
//		for (; flags[slot].value == 0;)
//		{
//		}
//	}
//
//	void unlock()
//	{
//		int64_t slot = reinterpret_cast<uint32_t>(TlsGetValue(tls_index));
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
//	std::atomic<int64_t>	tail;
//	int32_t					tls_index;
//};
//
