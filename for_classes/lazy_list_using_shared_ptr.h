#pragma once
// optimistic syncronization
// lazy syncronization

#include <mutex>
#include <atomic>
#include <iostream>
#include <memory>

using namespace std;
//

class SPLazyList
{
public:
	struct LNode
	{
		LNode() : removed{ false }, key{-1}
		{}

		LNode(int k) : removed{ false }, key{ k }
		{}

		void lock()
		{
			mtx.lock();
		}

		void unlock()
		{
			mtx.unlock();
		}


		shared_ptr<LNode>	next;
		int					key;
		bool				removed;
		mutex				mtx;
	};


public:
	SPLazyList()
	{
		head = make_shared<LNode>(0x8000'0000);
		tail = make_shared<LNode>(0x7FFF'FFFF);
		head->next = tail;
	}
	~SPLazyList()
	{}

	void clear()
	{
		head->next = tail;
	}


	bool is_valid(const shared_ptr<LNode>& pred, const shared_ptr<LNode>& curr)
	{
		return !pred->removed && !curr->removed && atomic_load(&pred->next) == curr;
	}


	bool add(int key)
	{
		for (;;)
		{
			shared_ptr<LNode> pred = head;
			shared_ptr<LNode> curr = atomic_load(&pred->next);

			while (curr->key < key)
			{
				pred = curr;
				curr = atomic_load(&curr->next);
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
				shared_ptr<LNode> new_node = make_shared<LNode>(key);
				new_node->next = curr;
				atomic_store(&pred->next ,new_node);
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
			shared_ptr<LNode> pred = head;
			shared_ptr<LNode> curr = atomic_load(&pred->next);

			while (curr->key < key)
			{
				pred = curr;
				curr = atomic_load(&curr->next);
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
				atomic_store(&pred->next, atomic_load(&curr->next));
				curr->unlock();
				pred->unlock();

				return true;
			}
		}
	}

	bool contain(int key)
	{
		shared_ptr<LNode> curr = head;

		while (curr->key < key)
		{
			curr = atomic_load(&curr->next);
		}

		return curr->key == key && curr->removed == false;
	}

	void display20()
	{
		shared_ptr<LNode> cur_ptr = head->next;
		for (int i = 0; i < 20; ++i)
		{
			if (tail == cur_ptr)
			{
				break;
			}

			cout << cur_ptr->key << ", ";
			cur_ptr = cur_ptr->next;
		}
		cout << endl;
	}


private:
	shared_ptr<LNode> head;
	shared_ptr<LNode> tail;
};