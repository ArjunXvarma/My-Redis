#pragma once
#include <queue>
#include <mutex>
#include <optional>

class DeadFdQueue {
private:
    std::queue<int> queue;
    std::mutex mtx;

public:
    void enqueue(int fd);
    std::optional<int> try_dequeue();
};