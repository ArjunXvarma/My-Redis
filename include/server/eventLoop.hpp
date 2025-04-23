#pragma once

class EventLoop {
public:
    virtual void addSocket(int fd) = 0;
    virtual void removeSocket(int fd) = 0;
    virtual void run() = 0;

    virtual ~EventLoop() = default;
};
