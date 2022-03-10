#include <iostream>

#include "ThreadPool.h"

int fun()
{
    return 2;
}

int main()
{
    ThreadPool thread_pool;
    thread_pool.enqueue(fun);

    return 0;
}
