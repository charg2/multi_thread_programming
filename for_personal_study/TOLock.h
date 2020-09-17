#pragma once



class TOLock
{
public:
	TOLock()
	{
	}

	bool try_lock(uint64_t tick)
	{
		uint64_t start_tick = GetTickCount64();


		return true;
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