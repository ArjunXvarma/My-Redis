#pragma once
#ifdef __APPLE__

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>
#include <vector>
#include <sys/socket.h>
#include <fcntl.h>

#include "clientHandler.hpp"
#include "eventLoop.hpp"

class KQueueLoop : public EventLoop {
private:
    int kq;
    uintptr_t serverSocketFd;
    std::unordered_set<int> registered_fds;
    std::unordered_map<int, ClientHandler> clientHandlers;
    
public:
    KQueueLoop(int serverSocketFd);
    ~KQueueLoop();
    
    void addEvent(int fd, uint32_t events) override;
    void removeEvent(int fd) override;
    void run() override;
};

#endif