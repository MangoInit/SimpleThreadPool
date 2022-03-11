#include <vector>
#include <thread>
#include <mutex>
#include <future>
#include <queue>
#include <functional>
#include <condition_variable>

class ThreadPool {
   public:
    explicit ThreadPool(int threads = std::thread::hardware_concurrency());
    ~ThreadPool();

    template <class F, class... Args>
    std::future<std::invoke_result<F, Args...>> enqueue(F &&f, Args &&...args)
    {
        using return_type = std::invoke_result_t<F, Args...>;
//        static_assert(std::is_same<return_type, void>::value);

        auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<return_type> res = task->get_future();
//        static_assert(std::is_same< decltype(res), std::future<void>>::value);

        {
            std::unique_lock<std::mutex> lock(queue_mutex);

            if (stop)
                throw std::runtime_error("enqueue on stopped ThreadPool");

            tasks.emplace([task = std::move(task)] { (*task)(); });
        }
        condition.notify_one();
        return res;
    }

    template <class F, class... Args>
    auto Add(F&& f, Args&... args) -> std::future< decltype(f(args...))>
    {
        std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

        std::function<void()> wrapper_func = [task_ptr]() { (*task_ptr)(); };

        {
            std::unique_lock<std::mutex> lock(queue_mutex);

            if (stop)
                throw std::runtime_error("enqueue on stopped ThreadPool");

            tasks.emplace(wrapper_func);
        }
        condition.notify_one();
        return task_ptr->get_future();
    }

   private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

