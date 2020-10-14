#pragma once


struct null_mutex
{
	void lock()
	{
	}

	void unlock()
	{
	}
};
