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

		// wait free queue�� ���� ó��
		node_t* prev = static_cast<node_t*>(InterlockedExchangePointer((volatile PVOID*)&tail_pos, node));
		if (prev != nullptr) // ��ó���� tail_pos
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
			// ���� ���� �ϴ� �ٸ� �����尡 ���� ���.
			if ( node == InterlockedCompareExchangePointer((volatile PVOID*)&tail_pos , nullptr, node) )
			{
				return;
			}

			// ���� ���� ���̳� lock() ó����;
			for (; node->next == nullptr ;) {} 
		}

		node->next->locked = false;
		node->next = nullptr;
	}

private:
	inline static thread_local node_t* local_node { };
	node_t*	tail_pos;
};