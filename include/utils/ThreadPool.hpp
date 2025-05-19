#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <thread>
#include <mutex>
#include <queue>
#include <vector>

#include "Queue.hpp"

class ThreadPool {
public:
    explicit ThreadPool(size_t num_threads = std::thread::hardware_concurrency());
    ~ThreadPool();

    template <typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
        using return_type = std::invoke_result_t<F, Args...>;

        auto task_ptr = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task_ptr->get_future();
        {
            std::unique_lock<std::mutex> lock(m);
            task_queue.push([task_ptr]() { (*task_ptr)(); });
        }

        cv.notify_one();
        return res;
    }

private:
    size_t num_threads;
    std::vector<std::jthread> threads;
    Queue<std::function<void()>> task_queue;
    std::mutex m;
    std::condition_variable cv;
};