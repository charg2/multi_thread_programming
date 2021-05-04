#pragma once

#include <cstdint>
#include <Windows.h>

class CLHLock
{
private:
	struct QNode
	{
		bool	locked;
	};

public:
	CLHLock() 
	{
		tail_pos = new QNode{ false };
	}

	~CLHLock()
	{
		delete tail_pos;
	}

	void lock()
	{
		QNode* node = local_node;
		if (node == nullptr)
		{
			node = new QNode{ true };
			local_node = node;
		}
		node->locked = true;


		//QNode* prev = reinterpret_cast<QNode*>(InterlockedExchange((unsigned long long*)tail_pos, (unsigned long long)node));
		QNode* prev = static_cast<QNode*>(InterlockedExchangePointer((volatile PVOID*)&tail_pos, node));
		local_prev = prev;


		// spinning
		for (; prev->locked == true;)
		{}
	}

	void unlock()
	{
		QNode* node = local_node;
		node->locked = false;

		QNode* prev = local_prev;
		local_node = prev;
	}

private:
	QNode* tail_pos;

	inline static thread_local QNode* local_node{};
	inline static thread_local QNode* local_prev{};
};