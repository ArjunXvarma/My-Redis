#pragma once

#include <functional>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#include "clientHandler.hpp"
#include "utils/GlobalThreadPool.hpp"
#include "utils/DeadFdQueue.hpp"

#define MAX_EVENTS 1024

class EventLoop {
private:
    uintptr_t serverSocketFd;
public:
    virtual void addEvent(int fd, uint32_t events) = 0;
    virtual void removeEvent(int fd) = 0;
    virtual void run() = 0;

    static int setNonBlocking(int fd) {
        int flags = fcntl(fd, F_GETFL, 0);
        return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }

    virtual ~EventLoop() = default;
};
