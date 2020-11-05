#pragma once
// optimistic syncronization
// lazy syncronization

#include <mutex>
#include <atomic>
#include <iostream>

using namespace std;

struct Node
{
	Node* volatile next;
	int key;
	volatile bool removed;
	std::mutex mtx;
};





class LazyList
{
public:
	struct LNode
	{
		LNode()
		{
			removed = false;
		}

		LNode(int k)
		{
			removed = false;
			key = k;
		}

		void lock()
		{
			mtx.lock();
		}

		void unlock()
		{
			mtx.unlock();
		}


		LNode* next;
		int					key;
		bool				removed;
		std::mutex			mtx;
	};


public:
	LazyList() : head(0x8000'0000), tail(0x7FFF'FFFF)
	{
		head.next = &tail;
	}
	~LazyList()
	{
	}

	void clear()
	{
		LNode* ptr = head.next;
		while (ptr != &tail)
		{
			LNode* delete_node = ptr;
			ptr = ptr->next;
			delete delete_node;
		}
		head.next = &tail;
	}

	bool is_valid(LNode* pred, LNode* curr)
	{
		return !pred->removed && !curr->removed && pred->next == curr;
	}


	bool add(int key)
	{
		for (;;)
		{
			LNode* pred = &head;
			LNode* curr = pred->next;

			while (curr->key < key)
			{
				pred = curr;
				curr = curr->next;
			}

			pred->lock();
			curr->lock();

			if (is_valid(pred, curr) == false)
			{
				curr->unlock();
				pred->unlock();

				continue;
			}
			if (curr->key == key) // 중복 존재.
			{
				curr->unlock();
				pred->unlock();

				return false;
			}
			else
			{
				LNode* new_node = new LNode(key);
				new_node->next = curr;
				pred->next = new_node;
				curr->unlock();
				pred->unlock();

				return true;
			}
		}
	}



	bool remove(int key)
	{
		for (;;)
		{
			LNode* pred = &head;
			LNode* curr = pred->next;

			while (curr->key < key)
			{
				pred = curr;
				curr = curr->next;
			}

			pred->lock();
			curr->lock();

			if (is_valid(pred, curr) == false) // 유효하지 않은 상태.
			{
				curr->unlock();
				pred->unlock();

				continue;
			}

			if (curr->key != key) // 중복 존재.
			{
				curr->unlock();
				pred->unlock();

				return false;
			}
			else
			{
				curr->removed = true;// pred의 next 변경전에 순서중요!
				atomic_thread_fence(memory_order_seq_cst);
				pred->next = curr->next;
				curr->unlock();
				pred->unlock();

				return true;
			}
		}
	}

	bool contain(int key)
	{
		LNode* curr = &head;

		while (curr->key < key)
		{
			curr = curr->next;
		}

		return curr->key == key && curr->removed == false;
	}

	void display20()
	{
		LNode* cur_ptr = head.next;
		for (int i = 0; i < 20; ++i)
		{
			if (&tail == cur_ptr)
			{
				break;
			}

			cout << cur_ptr->key << ", ";
			cur_ptr = cur_ptr->next;
		}
		cout << endl;
	}


private:
	LNode head;
	LNode tail;
};
