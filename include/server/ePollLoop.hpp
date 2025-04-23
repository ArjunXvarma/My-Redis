#include "eventLoop.hpp"
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>

using namespace std;

class EPollLoop : EventLoop {
private:
    int epollFd;
public:
    EPollLoop();
    
    void addSocket(int fd) override;
    void removeSocket(int fd) override;
    void run() override;

    ~EPollLoop() override;
};
