#pragma once

#include <Windows.h>

#define assert_if_false(expr) \
	{ \
		if ( !(expr) ) \
		{ \
			int* dummy = 0 ; \
			*dummy = 0xDEADBEEF ; \
		} \
	}


namespace waitfree
{
	namespace mpmc
	{
		template <class T, size_t N = 16>
		class bounded_queue
		{
		public:
			static constexpr size_t power_of_two(size_t value)
			{
				if (value == 0)
				{
					return 1;
				}

				return power_of_two(value - 1) * 2;
			}

			/// 큐의 크기 설정: 반드시 2의 승수로 해야 한다.
			static inline constexpr size_t QUEUE_MAX_SIZE = power_of_two(N);
			static inline constexpr size_t QUEUE_SIZE_MASK = QUEUE_MAX_SIZE - 1;

			bounded_queue() : elements{}, head_pos{}, tail_pos{}
			{}

			void push(T* new_element)
			{
				size_t insert_pos = InterlockedIncrement64(&tail_pos) - 1;

				assert_if_false(insert_pos - head_pos < QUEUE_MAX_SIZE); ///< overflow

				elements[insert_pos & QUEUE_SIZE_MASK] = new_element;
			}

			T* pop()
			{
				T* pop_value = (T*)InterlockedExchangePointer((void**)&elements[head_pos & QUEUE_SIZE_MASK], nullptr);
				// nullptr이면 head를 변경하지 않는다.
				if (pop_value != nullptr)
				{
					InterlockedIncrement64(&head_pos);
				}

				return pop_value;
			}

			size_t unsafe_size() const
			{
				return tail_pos - head_pos;
			}


		private:
			T* elements[QUEUE_MAX_SIZE];
			volatile size_t	head_pos;
			volatile size_t	tail_pos;

		}; // class bounded_queue<T>

	}//mpmc


	namespace mpsc
	{
		template<class T>
		class queue
		{
		private:
			struct node_t
			{
				node_t* next;
				T		data;
			};

		public:
			queue() : tail{}, head{}
			{
				head = new node_t{nullptr, };
				tail = head;
				head->next = nullptr;
			}
			~queue()
			{
				for (; nullptr != head  ;)
				{
					node_t* to_delete = head;
					head = head->next;

					delete to_delete;
				}
			}

			void push(T src)
			{
				node_t* new_node = new node_t{ nullptr , src };

				node_t* prev_node = (node_t*)InterlockedExchangePointer((void* volatile*)&tail, new_node);

				prev_node->next = new_node;
			}

			bool try_pop(T& dest)
			{
				node_t* dummy_next = head->next;  // [] - []  

				if (nullptr == dummy_next)
				{
					return false;
				}
				else
				{
					dest = dummy_next->data;

					node_t* head_ptr = head;

					head = dummy_next;

					delete head_ptr;

					return true;
				}
			}

			const bool empty() const
			{
				return head->next == nullptr;
			}

		private:
			alignas(64) node_t* volatile tail;
			alignas(64) node_t*			 head;
		}; // class queue<T>

	
	
}// namespace mpsc
}// namespace waitfree


