//
//#include <atomic>
//#include <vector>
//#include <thread>
//#include <algorithm>
//#include <Windows.h>
//
//class bakery_lock
//{
//public:
//	bakery_lock()
//	{
//		size_t n = std::thread::hardware_concurrency();
//		bool* flags = new bool[n]{};
//		labels.resize(n);
//
//		tls_index = TlsAlloc();
//		if (tls_index == TLS_OUT_OF_INDEXES)
//		{
//			int* hazard_ptr{};
//			*hazard_ptr = 0;
//		}
//	}
//
//	~bakery_lock()
//	{
//		delete[] flags;
//
//		if (tls_index != TLS_OUT_OF_INDEXES)
//		{
//			TlsFree(tls_index);
//		}
//	}
//
//	void lock()
//	{
//		size_t number_thread = std::thread::hardware_concurrency();
//		int slot = (c++) % number_thread;
//		TlsSetValue(tls_index, (void*)slot);
//
//		flags[slot] = true;
//		labels[slot] = *std::max_element(begin(labels), end(labels)) + 1;
//
//		for (int k{}; k < number_thread; ++k) // ±â¾Æ
//		{
//			if (k == slot)
//			{
//				continue;
//			}
//
//			while (flags[k] == true && (labels[k] < labels[slot] || (labels[k] == labels[slot] && k < slot)))
//			{}
//
//			break;
//		}
//	}
//
//
//	void unlock()
//	{
//		int64_t slot = reinterpret_cast<int64_t>(TlsGetValue(tls_index));
//		flags[slot] = false;
//	}
//
//private:
//	bool*					flags;
//	std::vector<size_t>		labels;
//	std::atomic<long long>  c;
//	uint32_t				tls_index;
//
//};
