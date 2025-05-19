#include "utils/ThreadPool.hpp"

ThreadPool::ThreadPool(size_t num_threads)
    : num_threads(num_threads), stop(false) {
    for (size_t i = 0; i < num_threads; i++) {
        threads.emplace_back([this] {
            std::function<void()> task;

            while (true) {
                {
                    std::unique_lock<std::mutex> l(m);
                    cv.wait(l, [this] {
                        return !task_queue.empty() || stop;
                    });

                    if (stop) return;

                    task = std::move(task_queue.pop());
                }

                task();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> l(m);
        stop = true;
    }
    cv.notify_all();
}