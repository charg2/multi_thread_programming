#pragma once
#include <iostream>
#include <thread>
#include <atomic>
#include <vector>

using namespace std;

class LFUQueue
{
	struct node_t
	{
	public:
		int key;
		node_t* volatile next;
	};

	alignas(64) node_t *head;
	alignas(64) node_t *tail;

public:
	LFUQueue()
	{
		head = tail = new node_t{0, nullptr};
	}

	~LFUQueue()
	{
		clear();

		delete head;
	}


	void clear()
	{
		node_t *ptr;

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
		node_t* e = new node_t{ x, nullptr };

		for(;;)
		{

		}
		
		tail->next = e;
		tail = e;
	}

	int deq()
	{

		if (nullptr == head->next)
		{
			return -1;
		}

		node_t* temp = head;
		int result = temp->next->key;
		head = temp->next;


		delete temp;
		return result;
	}

	void display20()
	{
		node_t* ptr = head->next;

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
