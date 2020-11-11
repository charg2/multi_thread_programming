#pragma once
#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>

#include "null_mutex.h"

using namespace std;


class CUQueue
{
	class node
	{
	public:
		int key;
		node* volatile next;

		node() { next = nullptr; }

		node(int key)
		{
			next = nullptr;
			key = key;
		}

		~node() {}
	};

	alignas(64) node *head;
	alignas(64) node *tail;
/*
	alignas(64) null_mutex enq_lock;
	alignas(64) null_mutex deq_lock;
*/

	alignas(64) mutex enq_lock;
	alignas(64) mutex deq_lock;


public:
	CUQueue()
	{
		head = tail = new node(0);
	}

	~CUQueue()
	{
		clear();

		delete head;
	}


	void clear()
	{
		node *ptr;

		while (head->next != nullptr)
		{
			ptr = head->next;
			head->next = head->next->next;
			delete ptr;
		}
		tail = head;
	}

	void enq(int x)
	{
		node *e = new node();
		e->key = x;
		enq_lock.lock();
		tail->next = e;
		tail = e;
		enq_lock.unlock();
	}

	int deq()
	{
		deq_lock.lock();

		if (nullptr == head->next) 
		{
			deq_lock.unlock();

			return -1;
		}
		
		node* temp = head;
		int result = temp->next->key;
		head = temp->next;
		
		deq_lock.unlock();
		
		delete temp;
		return result;
	}

	void display20()
	{
		node* ptr = head->next;

		for (int i{}; i < 20; ++i)
		{
			if (ptr == nullptr)
			{
				break;
			}

			cout << ptr->key << " , ";
			ptr = ptr->next;
		}
		cout << "\n";
	}
};
