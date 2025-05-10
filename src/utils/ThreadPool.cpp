#include "utils/ThreadPool.hpp"

ThreadPool::ThreadPool(size_t num_threads) : stop(false) {
    for (size_t i = 0; i < num_threads; ++i) {
        workers.emplace_back([this](std::stop_token stopToken) {
            while (true) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(queueMutex);

                    condition.wait(lock, [this, &stopToken]() {
                        return stop || !tasks.empty() || stopToken.stop_requested();
                    });

                    if (stop && tasks.empty()) 
                        return; // Exit the thread if stop is true and no tasks remain
                    

                    if (stopToken.stop_requested()) 
                        return; // Exit the thread if a stop request is received
                    

                    task = std::move(tasks.front());
                    tasks.pop();
                }

                task();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
}

void ThreadPool::enqueue(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.emplace(std::move(task));
    }
    condition.notify_one();
}