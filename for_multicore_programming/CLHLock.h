#pragma once

#include <cstdint>
#include <Windows.h>

class CLHLock
{
private:
	struct QNode
	{
		bool locked;
	};

public:
	CLHLock() : pred_index{ TLS_OUT_OF_INDEXES }, node_index{ TLS_OUT_OF_INDEXES }
	{
		tail = new QNode{ false };

		pred_index = TlsAlloc();
		if (pred_index == TLS_OUT_OF_INDEXES)
		{
			int* hazard_ptr{};
			*hazard_ptr = 0;
		}

		node_index = TlsAlloc();
		if (node_index == TLS_OUT_OF_INDEXES)
		{
			int* hazard_ptr{};
			*hazard_ptr = 0;
		}
	}

	~CLHLock()
	{
		if (pred_index != TLS_OUT_OF_INDEXES)
		{
			TlsFree(pred_index);
		}

		if (node_index != TLS_OUT_OF_INDEXES)
		{
			TlsFree(node_index);
		}
	}

	void lock()
	{
		QNode* node = static_cast<QNode*>(TlsGetValue(node_index));
		if (node == nullptr)
		{
			node = new QNode{ false };
		}
		node->locked = true;

		QNode* pred = static_cast<QNode*>(InterlockedExchangePointer((volatile PVOID*)&tail, node));
		TlsSetValue(pred_index, pred);

		// spinning
		for (; pred->locked == true;)
		{}
	}

	void unlock()
	{
		QNode* node = static_cast<QNode*>(TlsGetValue(node_index));
		node->locked = false;

		QNode* prev = static_cast<QNode*>(TlsGetValue(pred_index));
		TlsSetValue(node_index, prev);
	}

private:
	QNode* tail;
	uint32_t pred_index;
	uint32_t node_index;
};