#pragma once
#include <atomic>
#include <iostream>
#include <vector>

using namespace std;

/*
Number of Threads = 1, Exec Time = 19648ms,
1, 2, 3, 4, 6, 7, 8, 12, 14, 16, 17, 21, 24, 27, 29, 31, 34, 36, 39, 41,
Number of Threads = 2, Exec Time = 10141ms,
2, 3, 4, 7, 8, 16, 18, 20, 23, 24, 26, 27, 31, 32, 33, 34, 35, 36, 38, 41,
Number of Threads = 4, Exec Time = 5289ms,
1, 4, 6, 10, 11, 14, 15, 16, 17, 19, 20, 22, 25, 26, 28, 29, 30, 31, 32, 33,
Number of Threads = 8, Exec Time = 2903ms,
2, 5, 7, 9, 10, 12, 13, 15, 16, 18, 19, 22, 23, 28, 30, 31, 34, 40, 42, 44,
*/

class LockFreeList
{
public:
	struct Node
	{
		Node() : next{}, key{ -1}
		{}

		Node(int k) : key{ k }, next{}
		{}

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

		// 현재의 노드의 넥스트가 바뀌지 않았다면 새로운 노드를 넥스트로 변경함.
		bool cas_next(Node* old_addr, Node* new_addr, bool old_mark, bool new_mark)
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


			return atomic_compare_exchange_strong((atomic_int*)&next, &old_value, new_value);
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
		for (;;)
		{
			bool	is_removed;
			Node* local_next = local_cur->get_next(&is_removed); // 넥스트와 마크의 사본을 동시에 얻음.


			while (is_removed == true) // 지웡할게 한개가아니라 앞으로도 계속있다.
			{
				if (local_pred->cas_next(local_cur, local_next, false, false) == false) // 실패면 충돌한것. 이미 다른 스레드에서 find하다가 지우던가 .
				{																// add에서 추가해서 next가 바뀜.
					goto retry;													// 실패시 누가 석세서를 변경한것. // 첨부터다시시작.
				}

				local_cur = local_next;
				local_next = local_cur->get_next(&is_removed);
			}

			// 
			if (local_cur->key >= key)
			{
				*pred = local_pred;
				*curr = local_cur;

				return;
			}
			else
			{
				local_pred = local_cur;
				local_cur = local_cur->get_next();
			}
		}
	}


	bool add(int key)
	{
		Node* new_node = new Node(key);

		for (;;)
		{
			Node* local_pred;
			Node* local_curr;
			find(key, &local_pred, &local_curr); // key에 맞는 위치의 pred curr을 구함.

			if (local_curr->key == key)
			{
				delete new_node;

				return false;
			}
			else
			{
				new_node->set_next(local_curr, false);
				if (local_pred->cas_next(local_curr, new_node, false, false))
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

			if (curr->key != key) // 없는 경우 종료
			{
				return false;
			}
			else
			{
				bool is_removed;
				Node* succ = curr->get_next(&is_removed);
				// is_removed 를 마킹 원자적으로
				// 이미 지워졋다면 pass;
				// 2단계에 걸쳐서 진행.
				// 1. 현재 지울 노드의 마킹을 한다.
				// succ를 가리키는 포인터에 마킹이 포함되어 있어서 succ를 가리키는 포인터
				if (curr->cas_next(succ, succ, false, true) == false) // 이미 지워진 경우.
				{
					continue; // 다시 find 호출해서 진짜 없는지 // 제거 표시면 지우라는 계시임.
				}

				// 내가 삭제 표시를 한게 맞다면...
				pred->cas_next(curr, succ, false, false);

				return true;
			}
		}
	}


	bool contain(int key)
	{
		bool is_removed = false;

		Node* curr = &head;

		while (curr->key < key)
		{
			curr = curr->get_next(&is_removed);
		}

		return curr->key == key && is_removed == false;
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

