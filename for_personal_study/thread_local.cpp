#include "thread_local.h"



int32_t threeadlocal::get_thread_id()
{
    static thread_local int32_t t_id = -1;

    if (t_id == -1)
    {
        t_id = 1;
    }

    return int32_t();
}
