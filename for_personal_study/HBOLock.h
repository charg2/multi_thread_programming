#ifndef __TATAS_H
#define __TATAS_H

#include <thread>
#include <atomic>

thread_local bool _thisThreadHashInitialized;
thread_local uint_fast64_t _thisThreadHash;

/**\brief A test-and-test-and-set spinlock.
 *
 * This lock is good for short, uncontended critical sections.
 * If contention is high, use an mcs_lock.
 * Long critical sections should use pthread_mutex_t.
 *
 * Tradeoffs are:
 *  - test-and-test-and-set locks: low-overhead but not scalable
 *  - queue-based locks: higher overhead but scalable
 *  - pthread mutexes : very high overhead and blocks, but frees up
 *  cpu for other threads when number of cpus is fewer than number of threads
 *
 *  \sa REFSYNC
 */
class alignas(64) tatas_lock
{
    /**\cond skip */
    std::atomic<uint_fast64_t> _holderThreadHash;
    uint_fast64_t _noThreadHash;
    /**\endcond skip */

public:
    tatas_lock()
    {
        std::thread::id noThread = std::thread::id();
        std::hash<std::thread::id> threadHasher;
        _noThreadHash = threadHasher(noThread);
        _holderThreadHash = _noThreadHash;
    }

private:
    // CC mangles this as __1cKtatas_lockEspin6M_v_
    /// spin until lock is free
    void spin()
    {
        while (_holderThreadHash != _noThreadHash);
    }

public:
    /// Try to acquire the lock immediately.
    bool try_lock()
    {
        if (!_thisThreadHashInitialized)
        {
            std::thread::id thisThread = std::this_thread::get_id();
            std::hash<std::thread::id> threadHasher;
            _thisThreadHash = threadHasher(thisThread);
            _thisThreadHashInitialized = true;
        }
        bool success = false;
        uint_fast64_t oldHolderThreadHash = _noThreadHash;
        if (_holderThreadHash.compare_exchange_strong(oldHolderThreadHash, _thisThreadHash, std::memory_order_acquire))
        {
            success = true;
        }
        return success;
    }

    /// Acquire the lock, spinning as long as necessary.
    void acquire()
    {
        // w_assert1(!is_mine());
        if (!_thisThreadHashInitialized)
        {
            std::thread::id thisThread = std::this_thread::get_id();
            std::hash<std::thread::id> threadHasher;
            _thisThreadHash = threadHasher(thisThread);
            _thisThreadHashInitialized = true;
        }
        uint_fast64_t oldHolderThreadHash = _noThreadHash;
        do
        {
            spin();
            oldHolderThreadHash = _noThreadHash;
        } while (!_holderThreadHash.compare_exchange_strong(oldHolderThreadHash, _thisThreadHash, std::memory_order_acquire));
        // w_assert1(is_mine());
    }

    /// Release the lock
    void release()
    {
        // w_assert1(is_mine()); // moved after the fence
        _holderThreadHash.store(_noThreadHash, std::memory_order_release);
    }

    /// True if this thread is the lock holder
    bool is_mine() const
    {
        if (!_thisThreadHashInitialized)
        {
            std::thread::id thisThread = std::this_thread::get_id();
            std::hash<std::thread::id> threadHasher;
            _thisThreadHash = threadHasher(thisThread);
            _thisThreadHashInitialized = true;
        }
        return _thisThreadHash == _holderThreadHash;
    }

};

/** Scoped objects to automatically acquire tatas_lock. */
//class tataslock_critical_section {
//public:
//    tataslock_critical_section(tatas_lock *lock) : _lock(lock) {
//        _lock->acquire();
//    }
//    ~tataslock_critical_section() {
//        _lock->release();
//    }
//private:
//    tatas_lock *_lock;
//};

#endif // __TATAS_H


//#pragma once
//
//
//#include <thread>
//#include <atomic>
//
//class HBOLock
//{
//public:
//	HBOLock() : flag{ 0 }
//	{
//	}
//
//	void lock()
//	{
//		std::thread::id id = std::thread::id();
//		std:hash
//	}
//
//	void unlock()
//	{
//	}
//
//private:
//	int flag;
//};
//
