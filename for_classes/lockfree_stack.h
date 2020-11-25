#pragma once
#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <chrono>

using namespace std;
using namespace chrono;

class node_t
{
public:
	int key;
	node_t* volatile next;

	node_t() : next{} 
	{}
	node_t(int key_value) : key{ key_value }, next{ nullptr }
	{}
	~node_t() {}
};

// 성긴동기화 스택
class coarse_grain_stack
{
	node_t* top;
	mutex	s_lock;

public:
	coarse_grain_stack() : top{}
	{}

	~coarse_grain_stack()
	{
		unsafe_clear();
	}

	void unsafe_clear()
	{
		while (nullptr != top)
		{
			node_t* to_delete = top;
			top = top->next;
			delete to_delete;
		}
	}

	void push(int key)
	{
		node_t* e = new node_t(key);
		s_lock.lock();
		e->next = top;
		top = e;
		s_lock.unlock();
	}

	int pop()
	{
		s_lock.lock();
		if (nullptr == top)
		{
			s_lock.unlock();
			return -1;
		}
		node_t* p = top;
		int value = p->key;
		top = top->next;
		s_lock.unlock();
		delete p;
		return value;
	}

	void unsafe_display20()
	{
		node_t* ptr = top;
		for (int i = 0; i < 20; ++i)
		{
			if (nullptr == ptr) break;
			cout << ptr->key << ", ";
			ptr = ptr->next;
		}
		cout << endl;
	}
};

class lockfree_stack
{
	node_t* volatile top;

public:
	lockfree_stack() : top{}
	{}

	~lockfree_stack()
	{
		unsafe_clear();
	}

	void unsafe_clear()
	{
		while (nullptr != top)
		{
			node_t* to_delete = top;
			top = top->next;
			delete to_delete;
		}
	}

	bool CAS(node_t* volatile* ptr, node_t* o_node, node_t* n_node)
	{
		return atomic_compare_exchange_strong(
			reinterpret_cast<volatile atomic_int*>(ptr),
			reinterpret_cast<int*>(&o_node),
			reinterpret_cast<int>(n_node));
	}

	void push(int key)
	{
		node_t* e = new node_t(key);

		while (true)
		{
			node_t* first = top;
			e->next = first;
			if (true == CAS(&top, first, e))
			{
				return;
			}
		}
	}

	int pop()
	{
		while (true)
		{
			node_t* first = top;
			if (nullptr == first)
			{
				return -1;
			}
			node_t* next = first->next;
			int value = first->key;

			if (top != first)
			{
				continue;
			}
			if (true == CAS(&top, first, next))
			{
				//delete first;
				return value;
				// ABA문제 때문에 delete는 하지 않는다
			}
		}
	}

	void unsafe_display20()
	{
		node_t* ptr = top;
		for (int i = 0; i < 20; ++i)
		{
			if (nullptr == ptr) break;
			cout << ptr->key << ", ";
			ptr = ptr->next;
		}
		cout << endl;
	}
};

// 백오프
class BACKOFF
{
	int minDelay, maxDelay;
	int limit;
public:
	void init(int min, int max)
	{
		minDelay = min;
		maxDelay = max;
		limit = min;
	}

	void backoff()
	{
		int delay = 0;
		if (limit != 0) delay = rand() % limit;
		//if (0 == limit) return;
		limit *= 2;
		if (limit > maxDelay) limit = maxDelay;

		//this_thread::sleep_for(chrono::microseconds(delay));


		int current, start;
		// 클럭 레지스터를 읽는 명령어 사용
		_asm RDTSC;
		_asm mov start, eax;
		do
		{
			_asm RDTSC;
			_asm mov current, eax;
		} while (current - start < delay);

		//   _asm mov eax, delay;
		//myloop:
		//   _asm dec eax;
		//   _asm jnz myloop;
	}
};

// 락프리백오프스택
class lockfree_backoff_stack
{
	BACKOFF bo;
	node_t* volatile top;
public:
	lockfree_backoff_stack() : top{}
	{
		bo.init(1, 1000);
	}
	~lockfree_backoff_stack()
	{
		unsafe_clear();
	}

	void unsafe_clear()
	{
		while (nullptr != top)
		{
			node_t* to_delete = top;
			top = top->next;
			delete to_delete;
		}
	}

	bool CAS(node_t* volatile* ptr, node_t* o_node, node_t* n_node)
	{
		return atomic_compare_exchange_strong(
			reinterpret_cast<volatile atomic_int*>(ptr),
			reinterpret_cast<int*>(&o_node),
			reinterpret_cast<int>(n_node));
	}

	void push(int key)
	{
		node_t* e = new node_t(key);

		while (true)
		{
			node_t* first = top;
			e->next = first;
			if (true == CAS(&top, first, e))
				return;
			bo.backoff();
		}
	}

	int pop()
	{
		while (true)
		{
			node_t* first = top;
			if (nullptr == first)
			{
				return -2; // EMPTY
			}
			node_t* next = first->next;
			int value = first->key;
			if (top != first) continue;
			if (true == CAS(&top, first, next))
			{
				//delete first;
				return value;
			}
			// ABA문제 때문에 delete는 하지 않는다

			bo.backoff();
		}
	}

	void unsafe_display20()
	{
		node_t* ptr = top;
		for (int i = 0; i < 20; ++i)
		{
			if (nullptr == ptr) break;
			cout << ptr->key << ", ";
			ptr = ptr->next;
		}
		cout << endl;
	}
};
