#pragma once


class CompositeLock
{
public:
	CompositeLock()
	{
	}

	void lock()
	{

	}

	void unlock()
	{
	}

private:
	struct QNode
	{
		QNode* prev;
	};


};