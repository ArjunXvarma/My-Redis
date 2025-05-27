#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class Queue {
public:
    void push(const T& val) {
        std::unique_lock<std::mutex> l(m);
        q.push(val);
        cv.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> l(m);
        cv.wait(l, [&] {
            return !q.empty();
        });
        T front = q.front();
        q.pop();
        return front;
    }

    bool empty() {
        std::unique_lock<std::mutex> l(m);
        return q.empty();
    }

    T front() {
        std::unique_lock<std::mutex> l(m);
        return q.front();
    }

private:
    std::queue<T> q;
    std::mutex m;
    std::condition_variable cv;
};