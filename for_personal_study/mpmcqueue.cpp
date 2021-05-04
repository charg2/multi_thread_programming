#pragma once

/****************************************************************************
 * Wait-Free MPMC Queue
 * Minimum requirement: Windows XP or Windows Server 2003
 * Author: @sm9kr
 * License type: GPL v3
 * Drawback: Fixed size, __int64 overflow-able
 ****************************************************************************/

 /// ��Ƽ������ ȯ�濡�� ���� ������ (wait-free) FIFO ������
 /// ��, ����ũ���, ť�� ������ ���� (������ �ϴ� ���ø����̼� Ư���� �°� ������ ū ũ��� ������ ��)
 /// �ε���(head, tail)�� 2^63�� �Ѿ�� ���� (�̰� ���������δ� �Ͼ�� ����� ��)


#define _ASSERTC(expr) \
	{ \
		if ( !(expr) ) \
		{ \
			int* dummy = 0 ; \
			*dummy = 0xDEADBEEF ; \
		} \
	}



template <class T>
class MPMCQueue
{
public:

	template<int E>
	struct PowerOfTwo
	{
		enum { value = 2 * PowerOfTwo<E - 1>::value };
	};

	template<>
	struct PowerOfTwo<0>
	{
		enum { value = 1 };
	};

	enum
	{
		/// ť�� ũ�� ����: �ݵ�� 2�� �¼��� �ؾ� �Ѵ�.
		QUEUE_MAX_SIZE = PowerOfTwo<16>::value,
		QUEUE_SIZE_MASK = QUEUE_MAX_SIZE - 1
	};

	MPMCQueue() : mHeadPos(0), mTailPos(0)
	{
		memset(mElem, 0, sizeof(mElem));
	}

	void Push(T* newElem)
	{
		__int64 insertPos = InterlockedIncrement64(&mTailPos) - 1;
		_ASSERTC(insertPos - mHeadPos < QUEUE_MAX_SIZE); ///< overflow

		mElem[insertPos & QUEUE_SIZE_MASK] = newElem;
	}

	T* Pop()
	{
		T* popVal = (T*)InterlockedExchangePointer((void**)&mElem[mHeadPos & QUEUE_SIZE_MASK], nullptr);

		if (popVal != nullptr)
			InterlockedIncrement64(&mHeadPos);

		return popVal;
	}

	__int64 GetSize() const
	{
		return mTailPos - mHeadPos;
	}



private:

	T* mElem[QUEUE_MAX_SIZE];
	volatile __int64	mHeadPos;
	volatile __int64	mTailPos;

};