#pragma once

#include <Windows.h>
class MCSLock
{
private:
	struct node_t
	{
		bool	locked;
		node_t*	next;
	};

public:
	MCSLock() : tail_pos{ nullptr }
	{}

	void lock()
	{
		node_t* node = local_node;
		if (node == nullptr)
		{
			node = new node_t{ true, nullptr };
			local_node = node;
		}

		// wait free queue의 테일 처럼
		node_t* prev = static_cast<node_t*>(InterlockedExchangePointer((volatile PVOID*)&tail_pos, node));
		if (prev != nullptr) // 맨처음엔 tail_pos
		{
			node->locked = true;
			prev->next = node;

			for (; node->locked ;) {}
		}
	}

	void unlock()
	{
		node_t* node = local_node;
		
		if (node->next == nullptr)
		{
			// 락에 접근 하는 다른 스레드가 없는 경우.
			if ( node == InterlockedCompareExchangePointer((volatile PVOID*)&tail_pos , nullptr, node) )
			{
				return;
			}

			// 락에 접근 중이나 lock() 처리중;
			for (; node->next == nullptr ;) {} 
		}

		node->next->locked = false;
		node->next = nullptr;
	}

private:
	inline static thread_local node_t* local_node { };
	node_t*	tail_pos;
};