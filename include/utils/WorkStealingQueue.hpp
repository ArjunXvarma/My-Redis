#pragma once

#include <deque>
#include "Queue.hpp"

#include "common.hpp"

class WorkStealingQueue {
private:
    typedef function_wrapper data_type;
    std::deque<data_type> queue;
    mutable std::mutex mutex;

public:
    WorkStealingQueue() {

    }

    WorkStealingQueue(const WorkStealingQueue &other) = delete;
    WorkStealingQueue& operator=(const WorkStealingQueue &other) = delete;

    void push(data_type&& data) {
        std::unique_lock<std::mutex> lock(mutex);
        queue.push_back(std::move(data));
    }

    bool empty() const {
        std::unique_lock<std::mutex> lock(mutex);
        return queue.empty();
    }

    bool try_pop(data_type &res) {
        std::unique_lock<std::mutex> lock(mutex);
        if (queue.empty())
            return false;

        res = std::move(queue.front());
        queue.pop_front();
        return true;
    }

    bool try_steal(data_type &res) {
        std::unique_lock<std::mutex> lock(mutex);
        if (queue.empty())
            return false;

        res = std::move(queue.back());
        queue.pop_back();
        return true;
    }
};