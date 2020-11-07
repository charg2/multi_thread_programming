#pragma once
#include <atomic>
#include <iostream>
#include <set>
#include <mutex>

using namespace std;

constexpr size_t MAX_THREAD = 8;
int thread_local thread_id = 0;

class SET_LIST
{
	set<int> set;
public:
	SET_LIST()
	{
	}
	~SET_LIST()
	{
	}

	void clear()
	{
		set.clear();
	}

	bool add(int x)
	{
		if (0 != set.count(x))
		{
			return false;
		}
		else
		{
			set.insert(x);
			return true;
		}
	}

	bool remove(int x)
	{
		if (0 == set.count(x))
		{
			return false;
		}
		set.erase(x);
		return true;
	}

	bool contain(int x)
	{
		return 0 != set.count(x);
	}



	void display20()
	{
		int counter{};

		for (const auto v : set)
		{
			cout << v << ", ";
			counter += 1;
			if (counter == 20)
			{
				break;
			}
		}

		cout << endl;
	}
};

class SET_CLIST
{
	set<int> set;
	mutex mtx;
public:
	SET_CLIST()
	{}

	~SET_CLIST()
	{
	}

	void clear()
	{
		set.clear();
	}

	bool add(int x)
	{
		lock_guard<mutex> guard(mtx);
		if (0 != set.count(x))
		{
			return false;
		}
		else
		{
			set.insert(x);
			return true;
		}
	}

	bool remove(int x)
	{
		lock_guard<mutex> guard(mtx);
		if (0 == set.count(x))
		{
			return false;
		}
		set.erase(x);

		return true;
	}

	bool contain(int x)
	{
		lock_guard<mutex> guard(mtx);
		return 0 != set.count(x);
	}

	void display20()
	{
		lock_guard<mutex> guard(mtx);

		int counter{};
		for (const auto v : set)
		{
			cout << v << ", ";
			counter += 1;
			if (counter == 20)
			{
				break;
			}
		}
		cout << endl;
	}

};


constexpr int ADD = 1;
constexpr int REMOVE = 2;
constexpr int CONTAIN = 3;
constexpr int CLEAR = 4;
constexpr int DISPLAY20 = 5;

struct Invocation
{
	int method_type;
	int param1;
};

struct Response
{
	bool resp1;
};

struct SeqObject_SET
{
	set<int> set;
public:
	Response apply(Invocation inv)
	{
		switch (inv.method_type)
		{
		case ADD:
			if (0 != set.count(inv.param1))
			{
				return Response{ false };
			}
			else
			{
				set.insert(inv.param1);

				return Response{ true };
			}
			break;

		case REMOVE:
			if (0 == set.count(inv.param1))
			{
				return Response{ false };
			}
			set.erase(inv.param1);
			return Response{ true };

		case CONTAIN:
			return Response{ 0 != set.count(inv.param1) };

		case CLEAR:
			set.clear();
			return Response{};

		case DISPLAY20:
			int counter{};
			for (const auto v : set)
			{
				cout << v << ", ";
				counter += 1;
				if (counter == 20)
				{
					break;
				}
			}
			cout << endl;
			return Response{};
		}
	}
};



class SEQ_SET_LIST
{
	SeqObject_SET set;
public:
	SEQ_SET_LIST()
	{}

	~SEQ_SET_LIST()
	{}

	void SET_SEQ_LIST()
	{
		set.apply(Invocation{ CLEAR, 0 });
	}

	void clear()
	{
		set.apply(Invocation{ CLEAR, 0 });
	}

	bool add(int x)
	{
		return set.apply(Invocation{ ADD, x }).resp1;
	}

	bool remove(int x)
	{
		return set.apply(Invocation{ REMOVE, x }).resp1;
	}


	bool contain(int x)
	{
		return set.apply(Invocation{ CONTAIN, x }).resp1;
	}

	void display20()
	{
		set.apply(Invocation{ DISPLAY20, 0 });
	}

};


struct UNODE;
struct CONSENSUS
{
	atomic_int next;

public:
	CONSENSUS() : next{ -1 }
	{}

	UNODE* decide(UNODE* val)
	{
		int v = reinterpret_cast<int> (val);
		int ov = -1;
	
		atomic_compare_exchange_strong(&next, &ov, v); // -1 일 경우만 변경.
		
		v = next;
		return reinterpret_cast<UNODE*>(v);
	}

};



struct UNODE
{
	Invocation inv;
	UNODE* next;
	int seq;
	CONSENSUS decide_next;

public:
	UNODE()
	{
		next = nullptr;
		seq = 0;
	}

	UNODE(Invocation inv)
	{
		this->inv = inv;
		next = nullptr;
		seq = 0;
	}
};



struct LFUniversal_SET
{
	UNODE  tail;
	UNODE* head[MAX_THREAD];

	LFUniversal_SET() //: tail{}, head{}
	{
		for (auto& h : head)
		{
			h = &tail;
		}
	}



	UNODE* get_new()
	{
		UNODE* new_node = head[0];

		for (const auto ptr : head)
		{
			if (new_node->seq < ptr->seq)
			{
				new_node = ptr;
			}
		}

		return new_node;
	}

public:
	Response apply(Invocation inv)
	{
		UNODE* prefer = new UNODE( inv );
		while (0 == prefer->seq)
		{
			UNODE* before = get_new();
			UNODE* after = before->decide_next.decide(prefer);
			before->next = after;
			after->seq = before->seq + 1;
			head[thread_id] = after; // 최신으로 갱신.
		}

		//if ( prefer->inv.method_type == DISPLAY20)
		//{
		//	cout << ".";
		//}

		//  최신 결과 반영된 셋을 만들어준다.
		SeqObject_SET s_set;
		UNODE* p = tail.next;

		while (p != prefer)
		{
			if (p->inv.method_type != DISPLAY20) // 내부에 display 20이 있다; 
			{
				s_set.apply(p->inv);
			}

			p = p->next;
		}

		return s_set.apply(inv);
	}
};



class LF_SET_LIST
{
	LFUniversal_SET set;

public:
	LF_SET_LIST()
	{
		//set.apply(Invocation{ CLEAR, 0 });
	}

	~LF_SET_LIST()
	{}

	void SET_SEQ_LIST()
	{
		set.apply(Invocation{ CLEAR, 0 });
	}

	void clear()
	{
		set.apply(Invocation{ CLEAR, 0 });
	}

	bool add(int x)
	{
		return set.apply(Invocation{ ADD, x }).resp1;
	}

	bool remove(int x)
	{
		return set.apply(Invocation{ REMOVE, x }).resp1;
	}

	bool contain(int x)
	{
		return set.apply(Invocation{ CONTAIN, x }).resp1;
	}

	void display20()
	{
		//cout << "\n\n\ndisplay20\n\n\n";
		set.apply(Invocation{ DISPLAY20, 0 });
	}
};

