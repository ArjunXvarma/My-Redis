#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <vector>
#include <thread>
#include <stop_token>
#include <future>

#include "Queue.hpp"

class ThreadPool {
public:
    ThreadPool(size_t num_threads = std::thread::hardware_concurrency());
    ~ThreadPool();

    template <typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::invoke_result<F, Args...>::type> {
        using return_type = typename std::invoke_result<F, Args...>::type;

        auto task_ptr = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task_ptr->get_future();
        {
            std::unique_lock<std::mutex> lock(m);
            if (stop) throw std::runtime_error("enqueue on stopped ThreadPool");
            task_queue.push([task_ptr]() { (*task_ptr)(); });
        }

        cv.notify_one();
        return res;
    }

private:
    size_t num_threads; // Declare num_threads before stop
    bool stop = false;  // Declare stop after num_threads
    std::vector<std::thread> threads;
    Queue<std::function<void()>> task_queue;
    std::mutex m;
    std::condition_variable cv;
};