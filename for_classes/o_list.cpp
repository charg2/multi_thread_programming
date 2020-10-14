#pragma once
// optimistic syncronization

#include <mutex>
#include <atomic>
#include <memory>

using namespace std;

struct Node
{
	Node* volatile next;
	int key;
	volatile bool removed;
	std::mutex mtx;
};



struct SPONode
{
	SPONode()
	{
		removed = false;
	}

	SPONode(int k)
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


	std::shared_ptr<SPONode>		next;
	int								key;
	bool							removed;
	std::mutex						mtx;
};



//class LazyList
//{
//public:
//	LazyList()
//	{
//		head.key = 0x8000'0000;
//		tail.key = 0x7FFF'FFFF;
//		head.next = &tail;
//	}
//	~LazyList()
//	{
//
//	}
//
//	bool is_valid(Node* pred, Node* curr)
//	{
//		return !pred->removed && !curr->removed && pred->next == curr;
//	}
//
//	bool add(int key)
//	{
//		Node* pred = &head;
//		Node* curr = pred->next;
//		
//		while (curr->key < key)
//		{
//			pred = curr;
//			curr = curr->next;
//		}
//
//		pred->mtx.lock();
//		curr->mtx.lock();
//		if (is_valid(pred, curr))
//		{
//			if (curr->key == key)
//			{
//				curr->mtx.unlock();
//				pred->mtx.unlock();
//				return false;
//			}
//			else
//			{
//				Node* new_node = new Node{};
//				new_node->key = key;
//				new_node->next = curr;
//				pred->next = new_node;
//				curr->mtx.unlock();
//				pred->mtx.unlock();
//				return true;
//			}
//		}
//
//		curr->mtx.unlock();
//		pred->mtx.unlock();
//
//		return false;
//	}
//
//	bool remove(int key)
//	{
//		Node* pred = &head;
//		Node* curr = pred->next;
//
//		while (curr->key < key)
//		{
//			pred = curr;
//			curr = curr->next;
//		}
//
//
//		pred->mtx.lock();
//		curr->mtx.lock();
//
//		if (is_valid(pred, curr))
//		{
//			if (curr->key != key)
//			{
//				pred->mtx.unlock();
//				curr->mtx.unlock();
//				return false;
//			}
//			else
//			{
//				pred->next = curr->next;
//
//				atomic_thread_fence(std::memory_order_seq_cst);
//
//				pred->mtx.unlock();
//				curr->mtx.unlock();
//				return true;
//			}
//		}
//		pred->mtx.unlock();
//		curr->mtx.unlock();
//
//		return false;
//	}
//
//	bool contain(int key)
//	{
//		Node* curr = &head;
//		while (curr->key < key)
//		{
//			curr = curr->next;
//		}
//
//		return curr->key == key && !curr->removed;
//	}
//
//private:
//	Node head;
//	Node tail;
//};
//



class SPOList
{
public:
	SPOList()
	{
		head = std::make_shared<SPONode>(0x8000'0000);
		tail = std::make_shared<SPONode>(0x7FFF'FFFF);

		head->next = tail;
	}
	~SPOList()
	{}

	void clear()
	{
		head->next = tail;
	}

	bool is_valid(const shared_ptr<SPONode>& pred, const shared_ptr<SPONode>& curr)
	{
		return !pred->removed && !curr->removed && pred->next == curr;
	}


	bool add(int key)
	{
		for(;;)
		{
			shared_ptr<SPONode> pred = head;
			shared_ptr<SPONode> curr = pred->next;

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
				shared_ptr<SPONode> new_node = make_shared<SPONode>(key);
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
			shared_ptr<SPONode> pred = head;
			shared_ptr<SPONode> curr = pred->next;

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

				// delete 하지 않는다.

				return true;
			}
		}
	}

	bool contain(int key)
	{
		shared_ptr<SPONode> curr = head;

		while (curr->key < key)
		{
			curr = curr->next;
		}

		return curr->key == key && curr->removed == false;
	}

private:
	std::shared_ptr<SPONode> head;
	std::shared_ptr<SPONode> tail;
};

