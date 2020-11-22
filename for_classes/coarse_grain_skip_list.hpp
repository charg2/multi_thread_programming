#pragma once


#include <mutex>
#include <iostream>

#include "fast_rand.hpp"


using namespace std;

template <size_t MAX_LEVEL = 10>
class coarse_grain_skip_list
{
	//static constexpr size_t MAX_LEVEL = 10;
	struct alignas(64) node_t
	{
		static_assert(MAX_LEVEL > 2, "MAX_HEIGHT must be greater than 2.");
		node_t* next[MAX_LEVEL];
		int		key;
		int		height;

		node_t() : next{}, key{}, height{ MAX_LEVEL }
		{}
		
		node_t(int my_key) : next{}, key{ my_key }, height{ MAX_LEVEL }
		{}
		
		node_t(int my_key, int height) : next{}, key{ my_key }, height{ height }
		{}
	};


private:
	alignas(64) node_t head;
	alignas(64) node_t tail;

	std::mutex mtx;
public:
	coarse_grain_skip_list() : head{ 0x80000000L }, tail{ 0x7FFFFFFFL }
	{
		for (auto& ptr : head.next)
		{
			ptr = &tail;
		}
	}

	~coarse_grain_skip_list()
	{
		unsafe_clear();
	}

	void unsafe_clear()
	{
		node_t* to_delete;
		while (head.next[0] != &tail)
		{
			to_delete = head.next[0];
			head.next[0] = head.next[0]->next[0];
			delete to_delete;
		}

		for (auto& ptr : head.next)
		{
			ptr = &tail;
		}
	}

	void find(int key, node_t* preds[MAX_LEVEL], node_t* currs[MAX_LEVEL])
	{
		int current_level = MAX_LEVEL - 1;

		for (;;)
		{
			if (MAX_LEVEL - 1 == current_level) // 최상위 층이면 헤드부터
			{
				preds[current_level] = &head;
			}
			else // 아니면 해당 레벨부터
			{
				preds[current_level] = preds[current_level + 1]; //
			}

			currs[current_level] = preds[current_level]->next[current_level];

			while ( currs[current_level]->key < key )
			{
				preds[current_level] = currs[current_level];
				currs[current_level] = currs[current_level]->next[current_level];
			}

			if (0== current_level)
			{
				return;
			}

			current_level -= 1;
		}
	}

	bool add(int key)
	{
		node_t* preds[MAX_LEVEL];
		node_t* currs[MAX_LEVEL];

		mtx.lock();
		find(key, preds, currs);

		if (key == currs[0]->key) // 중복 return;
		{ 
			mtx.unlock(); 
			return false; 
		}
		else
		{
			int height = 1; //높이가 0층은 없고 무조건 1층은 있어야 함. 


			// 랜덤하게 층을 정한다. 단 상위 층으로 가능 확률은 줄어든다.
			while ( fast_rand() % 2 == 0 ) ////위로 올라갈수록 확률이 줄어들어야한다. if문 10층 쌓기 싫으니까 while로 
			{ 
				height++; 

				if ( MAX_LEVEL == height )
				{
					break;
				}
			}

			node_t* node = new node_t{ key, height };
			
			//어떻게 끼워넣을거냐 
			for (int cur_height = 0; cur_height < height; ++cur_height) 
			{ 
				node->next[cur_height] = currs[cur_height]; 
				preds[cur_height]->next[cur_height] = node; 
			} 

			mtx.unlock(); 

			return true; 
		}
	}

	bool remove(int key)
	{
		node_t* preds[MAX_LEVEL];
		node_t* currs[MAX_LEVEL];

		mtx.lock();

		find(key, preds, currs);

		if (key == currs[0]->key)
		{
			//삭제 
			for (int i = 0; i < currs[0]->height; ++i) 
			{ 
				preds[i]->next[i] = currs[i]->next[i]; 
			} 
			
			delete currs[0]; 
			
			mtx.unlock(); 
			return true;
		}
		else
		{
			mtx.unlock();
			return false;
		}
	}

	bool contain(int key)
	{
		node_t* preds[MAX_LEVEL];
		node_t* currs[MAX_LEVEL]; 
		
		mtx.lock(); 
		
		find(key, preds, currs); 
		
		if (key == currs[0]->key) 
		{
			mtx.unlock(); 
			return true; 
		}
		else 
		{ 
			mtx.unlock(); 
			return false; 
		}
	}

	void unsafe_display20()
	{
		int c = 20; 
		node_t* p = head.next[0]; 
		
		while (p != &tail)
		{
			cout << p->key << ", ";

			p = p->next[0]; //밑바닥만 보여주자 지름길은 신경쓰지않고 
			c--; 
			
			if (c == 0)
			{
				break;
			}
		} 
		cout << endl;
	}
};
