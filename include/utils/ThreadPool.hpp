#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <vector>
#include <thread>
#include <stop_token>

class ThreadPool {
public:
    ThreadPool(size_t num_threads = std::thread::hardware_concurrency());
    ~ThreadPool();

    void enqueue(std::function<void()> task);

private:
    std::vector<std::jthread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;
};