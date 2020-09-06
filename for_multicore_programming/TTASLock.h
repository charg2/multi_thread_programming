#pragma once
class TTASLock
{
public:
	TTASLock();

	void lock();
	void unlock();

private:
	int flag;
};

