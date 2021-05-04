#pragma once
#include <atomic>

using namespace std;


namespace lockfree
{
	template<typename T>
	struct control_block
	{
		atomic<int> use_count;
		atomic<int> weak_count;
		T* ptr;
	};


	template<typename T>
	class weak_ptr
	{
	public:
		weak_ptr() : block{ nullptr }
		{
		}

		T* get()
		{
			return block.ptr;
		}

	private:
		control_block* block;
	};

	template<typename T>
	class shared_ptr
	{
	public:
		shared_ptr() : block{ nullptr }
		{
		}


	private:
		control_block* block;

		friend shared_ptr<T> make_shared(Args&&... other);
	};

	template<typename T, typename... Args>
	shared_ptr<T> make_shared(Args&&... other)
	{
		shared_ptr<T> ptr = new shared_ptr<T>(other);
		ptr.block = new control_block<T>{ 0, 0, new T };
		


		return shared_ptr{};
	}

};


