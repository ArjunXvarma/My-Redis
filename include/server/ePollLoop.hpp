#pragma once

#include "eventLoop.hpp"
#ifdef __linux__
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>
#include <functional>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unordered_set>

class EPollLoop : public EventLoop {
private:
    int epollFd;
    int serverSocketFd;
    std::unordered_set<int> activeSockets;
    std::unordered_map<int, ClientHandler> clientHandlers;
    
public:
    EPollLoop(int serverSocketFd);
    
    void addEvent(int fd, uint32_t events) override;
    void removeEvent(int fd) override;
    void run() override;

    ~EPollLoop() override;
};

#endif
