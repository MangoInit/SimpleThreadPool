#include <iostream>
#include <asm/unistd.h>
#include <unistd.h>
#include "ThreadPool.h"


int test()
{
//    sleep(1);
    return (int)syscall(__NR_gettid);
}

int main()
{
    ThreadPool thread_pool;
//    thread_pool.enqueue(fun);

    std::vector<std::future<int>> v;
    v.reserve(100);
    for (int i = 0; i < 100; ++i)
            v.emplace_back(thread_pool.Add(test));

    for (auto &it: v) std::cout << it.get() << std::endl;

    return 0;
}
