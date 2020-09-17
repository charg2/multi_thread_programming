#pragma once

#include <Windows.h>
class MCSLock
{
private:
	struct QNode
	{
		bool	locked;
		QNode*	next;
	};

public:
	MCSLock() : tail{ nullptr }
	{}

	void lock()
	{
		QNode* node = local_node;
		if (node == nullptr)
		{
			node = new QNode{ true, nullptr };
			local_node = node;
		}

		QNode* prev = static_cast<QNode*>(InterlockedExchangePointer((volatile PVOID*)&tail, node));

		if (prev != nullptr)
		{
			node->locked = true;
			prev->next = node;

			for (; node->locked ;) {}
		}
	}

	void unlock()
	{
		QNode* node = local_node;
		
		if (node->next == nullptr)
		{
			// 락에 접근 하는 다른 스레드가 없는 경우.
			if ( node == InterlockedCompareExchangePointer((volatile PVOID*)&tail , nullptr, node) )
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
	inline static thread_local QNode* local_node { };
	QNode*	tail;
};