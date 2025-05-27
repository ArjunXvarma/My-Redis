#include "utils/ThreadPool.hpp"

ThreadPool::ThreadPool(size_t num_threads)
    : num_threads(num_threads) {
    for (size_t i = 0; i < num_threads; i++) {
        threads.emplace_back([this](std::stop_token stoken) {
            while (!stoken.stop_requested()) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(m);
                    cv.wait(lock, [this, &stoken]() {
                        return !task_queue.empty() || stoken.stop_requested();
                    });

                    if (stoken.stop_requested() && task_queue.empty())
                        return;

                    if (task_queue.empty()) continue;  // spurious wakeup
                    task = std::move(task_queue.pop());
                }

                task();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    cv.notify_all();
}
