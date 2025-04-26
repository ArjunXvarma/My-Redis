#pragma once

#include <functional>

using namespace std;

class EventLoop {
public:
    virtual void addSocket(int fd) = 0;
    virtual void removeSocket(int fd) = 0;
    virtual void run(function<int(int)> eventHandler) = 0;

    virtual ~EventLoop() = default;
};
