#pragma once
#include <atomic>
#include <iostream>

#include <vector>
#include <thread>
#include <chrono>

using namespace std;
using namespace std::chrono;

class LockFreeList
{
public:

	struct Node
	{
		Node()
		{
			next = 0;
		}

		Node(int k)
		{
			key = k;
			next = 0;
		}

		~Node()
		{
		}

		void set_next(Node* addr, bool is_removed)
		{
			int value = reinterpret_cast<int>(addr);
			if (true == is_removed)
			{
				value = value | 1;
			}
			//else  주소는 원래 0

			next = value; //  한번에 업데이트. 중간값 
		}

		Node* get_next()
		{
			int local_next = next;

			return reinterpret_cast<Node*>(local_next & 0xFFFF'FFFE);
		}

		Node* get_next(bool* is_removed)
		{
			int value = next;

			*is_removed = value & 1;

			return reinterpret_cast<Node*>(value & 0xFFFF'FFFE);
		}

		Node* cas_next(Node* old_addr, Node* new_addr, bool old_mark, bool new_mark)
		{
			int old_value = reinterpret_cast<int>(old_addr);
			if (true == old_mark)
			{
				old_value = old_value | 1;
			}
			int new_value = reinterpret_cast<int>(new_addr);
			if (true == new_mark)

			{
				new_value = new_value | 1;
			}

			// curr = curr->get_next();
			// 
			return atomic_compare_exchange_strong((volatile atomic<Node>*)this, (Node*)&old_value, reinterpret_cast<Node*>(new_value));
		}

		std::atomic_int		next;
		int					key;
	};





public:

	LockFreeList() : head(0x8000'0000), tail(0x7FFF'FFFF)
	{
		head.set_next(&tail, false);
	}

	~LockFreeList()
	{
		clear();
	}

	void clear()
	{
		Node* ptr = head.get_next();

		while (ptr != &tail)
		{
			Node* to_delete = ptr;
			ptr = ptr->get_next();

			delete to_delete;
		}

		head.set_next(&tail, false);

	}

	void find(int key, Node** pred, Node** curr)
	{
	retry:

		Node* local_pred = &head;
		Node* local_cur = local_pred->get_next();
		//  local_cur가 마킹되어있으면제거하고 local_cur를 다시 세팅

		bool is_removed;
		Node* su = local_cur->get_next(&is_removed);

		while (true == is_removed) // 지웡할게 한개가아니라 앞으로도 계속있다.
		{
			if (local_pred->cas_next(local_cur, su, false, false) == false) // 실패면 충돌한것. 
			{
				// 실패시 누가 석세서를 변경한것.
				// 첨부터다시시작.
				goto retry;
			}

			local_cur = su;
			su = local_cur->get_next(&is_removed);
		}


		if (local_cur->key >= key)
		{
			*pred = local_pred;
			*curr = local_cur;

			return;
		}

		local_pred = local_cur;
		local_cur = local_cur->get_next();
	}


	bool add(int key)
	{
		for (;;)
		{
			Node* pred;
			Node* curr;
			find(key, &pred, &curr); // key에 맞는 pred curr을 구함.

			if (curr->key == key)
			{
				return false;
			}
			else
			{
				Node* new_node = new Node(key);
				new_node->set_next(curr, false);
				if (pred->cas_next(curr, new_node, false, false))
				{
					return true;
				}
			}
		}
	}




	bool remove(int key)
	{
		
		for (;;)
		{
			Node* pred;
			Node* curr;
			find(key, &pred, &curr); // key에 맞는 pred curr을 구함.

			if (curr->key != key) 
			{
				return false;
			}
			else
			{
				bool is_removed;
				Node* succ = curr->get_next(&is_removed);
				// is_removed 를 마킹 원자적으로
				// 이미 지워졋다면 pass;
				
				if (is_removed == true) // 간단한 비교
				{
					// 이미 누가 한것.
					return false;
				}

				
				if (curr->get) // cas를 통해 마킹 확인.
				{

				}
				
				// 아니라면 지우고 return true;
				// 위에작업을 cas로

				
				if (false == is_removed)
				{
					continue;
				}

				return true;
			}
		}
	}


	bool contain(int key)
	{
		Node* curr = &head;

		Node* curr;
		Node* pred;

		find(key, &pred, &curr);

		while (curr->key < key)
		{
			curr = curr->get_next();
			curr->cas_next( next;
		}



		return curr->key == key && curr->removed == false;

	}


	void display20()
	{
		Node* cur_ptr = head.get_next();

		for (int i = 0; i < 20; ++i)
		{
			if (&tail == cur_ptr)
			{
				break;
			}

			cout << cur_ptr->key << ", ";

			cur_ptr = cur_ptr->get_next();
		}

		cout << endl;
	}





private:
	Node head;
	Node tail;
};



constexpr size_t test_case = 400'0000;
constexpr size_t key_range = 1000;
LockFreeList set;


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

		default:
			cout << "Error\n";
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

		auto start_time = high_resolution_clock::now();

		for (int n = 0; n < thread_count; ++n)

			threads.emplace_back(do_benchmark, thread_count);



		for (auto& t : threads)

			t.join();



		auto end_time = high_resolution_clock::now();

		auto exec_time = end_time - start_time;

		cout << "Number of Threads = " << thread_count << ", ";

		cout << "Exec Time = " << duration_cast<milliseconds>(exec_time).count() << "ms, \n";

		set.display20();

		set.clear();

	}

}
