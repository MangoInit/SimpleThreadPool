//
// Created by Mango Huang on 2022/3/10.
//

#include <vector>
#include <thread>
#include <mutex>
#include <queue>

class ThreadPool {
   public:
    explicit ThreadPool(int threads = std::thread::hardware_concurrency());

    template <class F, class... Args>
    auto enqueue(F &&f, Args &&...args);

    ~ThreadPool();

   private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

