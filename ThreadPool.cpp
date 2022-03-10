//
// Created by Mango Huang on 2022/3/10.
//

#include <future>
#include "ThreadPool.h"

inline ThreadPool::ThreadPool(int threads) : stop(false)
{
    for (size_t i = 0; i < threads; ++i)
        workers.emplace_back([this] {
            for (;;) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    this->condition.wait(
                        lock, [this] { return this->stop || !this->tasks.empty(); });
                    if (this->stop && this->tasks.empty())
                        return;
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }

                task();
            }
        });
}

template<class F, class... Args>
auto ThreadPool::enqueue(F &&f, Args &&...args)
{
    using return_type = std::invoke_result_t<F, Args...>;
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        if (stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks.emplace([task = std::move(task)] { (*task)(); });
    }
    condition.notify_one();
    return res;
}

inline ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread &worker: workers)
        worker.join();
}