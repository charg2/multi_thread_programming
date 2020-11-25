#pragma once

#include <atomic>
#include <iostream>


using namespace std;


constexpr int EMPTY_ST = 0;
constexpr int WAITING_ST = 1;
constexpr int BUSY_ST = 2;
// 교환자

constexpr int MAX_THREADS = 16;
extern size_t num_thread;
extern int el_success;


class EXCHANGER
{
	atomic_int slot;
	int get_state()
	{
		int t = slot;
		return (t >> 30) & 0x3;
	}
	int get_value()
	{
		int t = slot;
		return t & 0x3FFFFFFF;
	}
	bool CAS(int old_st, int new_st, int old_v, int new_v)
	{
		int ov = old_st << 30 + (old_v & 0x3FFFFFFF);
		int nv = new_st << 30 + (new_v & 0x3FFFFFFF);
		bool ret = atomic_compare_exchange_strong(&slot, &ov, nv);
		//old_v = ov & 0x3FFFFFFF;

		return ret;
	}
public:
	EXCHANGER() : slot{}
	{
	}
	~EXCHANGER() {}

	int exchange(int value, bool* time_out, bool* is_busy)
	{
		for (int i = 0; i < 100; ++i)
		{
			switch (get_state())
			{
			case EMPTY_ST:
			{
				if (true == CAS(EMPTY_ST, WAITING_ST, 0, value))  // waiting 상태로 변경 성공하면 
				{
					for (int n{}; n < 1000; ++n)
					{
						if (BUSY_ST != get_state())
						{
							// 교환 완료. 
							// busy로 교환 완료.
							int ret = get_value();
							slot = 0; // empty로 변경.
							return ret;
						}
					}

					if (true == CAS(WAITING_ST, EMPTY_ST, value, 0))  // 아무도 안왔다면 waiting -> empty로 변경
					{
						*time_out = true;

						return 0;
					}
					else // 다른스레드가 교환 완료햇다는것.
					{
						// busy로 변경되어 성공시 
						int ret = get_value();
						slot = 0; // empty로 변경.

						return ret;
					}
				}
				else // 실패  // 다른 스레드가 이미 CAS를 성공한 경우이다.
				{		      // 다른 스레드가 waiting으로 변경
					continue; // 처음부터 다시 시도
				}
			}
			break;

			case WAITING_ST:
			{
				int ret = get_value();
				if (true == CAS(WAITING_ST, BUSY_ST, ret, value))
				{
					++el_success;
					return ret;
				}
				else
				{
					continue;
				}
			}
			break;

			case BUSY_ST:
			{
				*is_busy = true;
				continue;
			}
			}
		}

		*time_out = true;
		return 0;
	}
};



class ELIMINATION_ARRAY
{
	int range;
	EXCHANGER ex[1 + MAX_THREADS / 2];
public:

	ELIMINATION_ARRAY() : range{ 1 }
	{
	}

	int visit(int value, bool* time_out)
	{
		int slot = rand() % range;
		bool is_busy = false;

		int ret = ex[slot].exchange(value, time_out, &is_busy);

		if ((true == *time_out) && (range > 2))
		{
			range--;
		}

		if (true == is_busy && range < num_thread / 2)
		{
			range++;
		}

		return ret;
	}
};

class lockfree_elemination_stack
{
	struct node_t
	{
	public:
		int key;
		node_t* volatile next;

		node_t() : next{}, key{}
		{}
		node_t(int key_value) : key{ key_value }, next{ nullptr }
		{}
	};


	node_t* volatile	top;
	ELIMINATION_ARRAY	el;

public:
	lockfree_elemination_stack()
	{
		top = nullptr;
	}
	~lockfree_elemination_stack()
	{
		unsafe_clear();
	}

	void unsafe_clear()
	{
		while (nullptr != top)
		{
			node_t* to_delete = top;
			top = top->next;
			delete to_delete;
		}
	}

	bool CAS(node_t* volatile* ptr, node_t* o_node, node_t* n_node)
	{
		return atomic_compare_exchange_strong(
			reinterpret_cast<volatile atomic_int*>(ptr),
			reinterpret_cast<int*>(&o_node),
			reinterpret_cast<int>(n_node));
	}

	void push(int key)
	{
		node_t* e = new node_t(key);

		while (true)
		{
			node_t* first = top;
			e->next = first;
			if (true == CAS(&top, first, e))
				return;
			bool time_out = false;
			int ret = el.visit(key, &time_out);
			if ((false == time_out) && (-1 == ret))
				return;
		}
	}

	int pop()
	{
		while (true)
		{
			node_t* first = top;
			if (nullptr == first)
			{
				return -1;
			}
			node_t* next = first->next;
			int value = first->key;
			if (top != first) continue;
			if (true == CAS(&top, first, next))
			{
				//delete first;
				return value;
				// ABA문제 때문에 delete는 하지 않는다
			}
		}
	}

	void unsafe_display20()
	{
		node_t* ptr = top;
		for (int i = 0; i < 20; ++i)
		{
			if (nullptr == ptr) break;
			cout << ptr->key << ", ";
			ptr = ptr->next;
		}
		cout << endl;
	}
};
