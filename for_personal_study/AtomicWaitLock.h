#pragma once
#include <atomic>

class AtomicWaitLock
{
public:
    AtomicWaitLock()
    :
    state{ false }
    {
    }

    void lock() noexcept
    {
        while( std::atomic_exchange( &state, true ) == true )
        {
            std::atomic_wait( &state, true );
        }
        //std::atomic_wait( &state, true );
    }

    void unlock() noexcept
    {
        state = false;
        std::atomic_notify_one( &state );
    }


private:
    std::atomic<bool> state;
};

