#pragma once
// optimistic syncronization

#include <mutex>
#include <atomic>
#include <memory>

#include <iostream>
#include <vector>
#include <chrono>

using namespace std::chrono;

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



class SPLazyList
{
public:
	SPLazyList()
	{
		head = std::make_shared<SPONode>(0x8000'0000);
		tail = std::make_shared<SPONode>(0x7FFF'FFFF);

		head->next = tail;
	}
	~SPLazyList()
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

	void display20()
	{
		shared_ptr<SPONode> cur_ptr = head->next;
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
	std::shared_ptr<SPONode> head;
	std::shared_ptr<SPONode> tail;
};


constexpr size_t test_case = 40'0000;
constexpr size_t key_range = 1000;
SPLazyList	  set;



void do_benchmark(int num_threads)
{
	for (int i = 0; i < test_case / num_threads; ++i)
	{
		switch (rand() % 3)
		{
		case 0:
			set.add(rand() % key_range);
			break;
		case 1:
			set.remove(rand() % key_range);
			break;
		case 2:
			set.contain(rand() % key_range);
			break;
		default: cout << "Error\n";
			exit(-1);
		}
	}
}

auto main() -> int
{
	set.clear();

	for (size_t thread_count = 1; thread_count <= 8; thread_count *= 2)
	{
		vector<thread> threads;
		auto start_t = high_resolution_clock::now();
		for (int n = 0; n < thread_count; ++n)
			threads.emplace_back(do_benchmark, thread_count);

		for (auto& t : threads)
			t.join();

		auto end_t = high_resolution_clock::now();
		auto exec_time = end_t - start_t;
		cout << "Number of Threads = " << thread_count << ", ";
		cout << "Exec Time = " << duration_cast<milliseconds>(exec_time).count() << "ms, \n";
		set.display20();
		set.clear();
	}
}