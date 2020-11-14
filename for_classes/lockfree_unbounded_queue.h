#pragma once
#include <iostream>

#include "lockfree.h"

using namespace std;

//class lockfree_unbounded_queue
//{
//	struct alignas(64) node_t
//	{
//		int					key;
//		node_t* volatile	next;
//	};
//
//	alignas(64) node_t* volatile head;
//	alignas(64) node_t* volatile tail;
//
//
//public:
//
//	lockfree_unbounded_queue()
//	{
//		head = tail = new node_t{ 0, nullptr };
//	}
//
//	~lockfree_unbounded_queue()
//	{
//		unsafe_clear();
//
//		delete head;
//	}
//
//
//	void unsafe_clear()
//	{
//		node_t* ptr;
//
//		while (head->next != nullptr)
//		{
//			ptr = head->next;
//			head->next = head->next->next;
//			delete ptr;
//		}
//		tail = head;
//	}
//
//	void enqueue(int x)
//	{
//		node_t* new_node = new node_t{ x, nullptr };
//
//		for (;;)
//		{
//			node_t* local_tail = this->tail;
//			node_t* local_next = local_tail->next;
//
//			if (local_tail != tail)
//			{
//				continue;
//			}
//
//			if (nullptr == local_next)
//			{
//				if (true == cas((volatile int*)&local_tail->next, (int)nullptr, (int)new_node))
//				{
//					// 위에서 캡처한 값일 경우만 꼬리 밀기.
//					cas((volatile int*)&tail, (int)local_tail, (int)new_node);//  꼬리 밀기
//					// 실패해도 누군가는 밀었다는것.
//					return;
//				}
//			}
//			else
//			{
//				cas((volatile int*)&tail, (int)local_tail, (int)local_next);//   // 꼬리 밀기.
//			}
//		}
//	}
//
//	int dequeue()
//	{
//		for (;;)
//		{
//			node_t* local_head = this->head;
//			node_t* local_tail = this->tail;
//			node_t* local_next = local_head->next;
//
//			if (local_head != head)
//			{
//				continue;
//			}
//
//			if (nullptr == local_next) // empty()
//			{
//				return -1;
//			}
//
//			if (local_head == tail) // 안 밀린경우 기다리지 말고 밀어준다.
//			{
//				cas((volatile int*)&tail, (int)local_tail, (int)local_tail->next);//   // 꼬리 밀기.
//				continue;
//			}
//
//			int ret_val = local_next->key;
//			if (false == cas((volatile int*)&head, (int)local_head, (int)local_next))
//			{
//				continue;
//			}
//
//			delete local_head;
//
//			return ret_val;
//		}
//	}
//
//	void unsafe_display20()
//	{
//		node_t* ptr = head->next;
//
//		for (int i{}; i < 20; ++i)
//		{
//			if (ptr == nullptr)
//			{
//				break;
//			}
//
//			cout << ptr->key << " , ";
//			ptr = ptr->next;
//		}
//		cout << "\n";
//	}
//
//
//	bool unsafe_empty()
//	{
//		return false;
//	}
//};

class lockfree_unbounded_queue
{
	struct alignas(64) node_t
	{
		int					key;
		node_t* volatile	next;
	};


	struct stamp_ptr
	{

		stamp_ptr() : ptr{}
		{}

		stamp_ptr(node_t* p, int stamp)
		{
			ptr = reinterpret_cast<int>(p) + stamp << 32;
		}

		void set_ptr(node_t* p, int stamp)
		{
			ptr = reinterpret_cast<int>(p) + stamp << 32;
		}

		node_t* get_addr()
		{
			return reinterpret_cast<node_t*>(ptr);
		}

		node_t* get_addr(int* stamp)
		{
			long long local_ptr = ptr; // ptr capture
			
			*stamp = local_ptr >> 32;

			return reinterpret_cast<node_t*>(local_ptr);
		}

		long long ptr;
	};



	bool cas_stamp(stamp_ptr* next, node_t* old_p, node_t* new_p, int old_st, int new_st)
	{
		stamp_ptr old_v ( old_p, old_st );
		stamp_ptr new_v ( new_p, new_st );


		return atomic_compare_exchange_strong(
			reinterpret_cast<volatile atomic_llong*>(next->ptr),
			reinterpret_cast<long long*>(&old_v.ptr),
			reinterpret_cast<long long>(new_v.ptr));
	}

	alignas(64) stamp_ptr head;
	alignas(64) stamp_ptr tail;


public:

	lockfree_unbounded_queue()
	{
		node_t* p = new node_t{ 0, nullptr };
		head = tail = stamp_ptr(p, 0);
	}

	~lockfree_unbounded_queue()
	{
		unsafe_clear();

		delete head.get_addr();
	}


	void unsafe_clear()
	{
		while (head.get_addr() != tail.get_addr())
		{
			node_t* do_delete = head.get_addr();
			head.set_ptr(head.get_addr()->next, 0);
			delete do_delete;
		}
	}

	void enqueue(int x)
	{
		node_t* new_node = new node_t{ x, nullptr };

		for (;;)
		{
			node_t* local_tail = this->tail;
			node_t* local_next = local_tail->next;

			if (local_tail != tail)
			{
				continue;
			}

			if (nullptr == local_next)
			{
				if (true == cas((volatile int*)&local_tail->next, (int)nullptr, (int)new_node))
				{
					// 위에서 캡처한 값일 경우만 꼬리 밀기.
					cas((volatile int*)&tail, (int)local_tail, (int)new_node);//  꼬리 밀기
					// 실패해도 누군가는 밀었다는것.
					return;
				}
			}
			else
			{
				cas((volatile int*)&tail, (int)local_tail, (int)local_next);//   // 꼬리 밀기.
			}
		}
	}

	int dequeue()
	{
		for (;;)
		{
			node_t* local_head = this->head;
			node_t* local_tail = this->tail;
			node_t* local_next = local_head->next;

			if (local_head != head)
			{
				continue;
			}

			if (nullptr == local_next) // empty()
			{
				return -1;
			}

			if (local_head == tail) // 안 밀린경우 기다리지 말고 밀어준다.
			{
				cas((volatile int*)&tail, (int)local_tail, (int)local_tail->next);//   // 꼬리 밀기.
				continue;
			}

			int ret_val = local_next->key;
			if (false == cas((volatile int*)&head, (int)local_head, (int)local_next))
			{
				continue;
			}

			delete local_head;

			return ret_val;
		}
	}

	void unsafe_display20()
	{
		node_t* ptr = head->next;

		for (int i{}; i < 20; ++i)
		{
			if (ptr == nullptr)
			{
				break;
			}

			cout << ptr->key << " , ";
			ptr = ptr->next;
		}
		cout << "\n";
	}


	bool unsafe_empty()
	{
		return false;
	}
};

