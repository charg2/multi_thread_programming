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
	{
	}

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
			// \
			if (static_cast<QNode*>(InterlockedExchangePointer((volatile PVOID*)&tail, node)))
			

			if (static_cast<QNode*>(InterlockedExchangePointer((volatile PVOID*)&tail, node)))
			{
				return;
			}

			for (; node->next == nullptr ;) {}

			node->next->locked = false;
			node->next = nullptr;

		}
	}


private:
	inline static thread_local QNode* local_node { };
	QNode*	tail;
};