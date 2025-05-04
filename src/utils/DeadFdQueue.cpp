#include "utils/DeadFdQueue.hpp"

void DeadFdQueue::enqueue(int fd) {
    std::lock_guard<std::mutex> lock(mtx);
    queue.push(fd);
}

std::optional<int> DeadFdQueue::try_dequeue() {
    std::lock_guard<std::mutex> lock(mtx);
    if (queue.empty()) return std::nullopt;
    int fd = queue.front();
    queue.pop();
    return fd;
}
