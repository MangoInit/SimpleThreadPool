#include <iostream>
#include "ThreadPool.h"

void fun()
{
}

int main()
{

    ThreadPool thread_pool;
    thread_pool.enqueue(fun);

    return 0;
}
