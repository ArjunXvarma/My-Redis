#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>
#include <functional>
#include <sys/epoll.h>
#include <stdexcept>

#include "eventLoop.hpp"

using namespace std;

class EPollLoop : public EventLoop {
private:
    int epollFd;
    int serverSocketFd;
public:
    EPollLoop(int serverSocketFd);
    
    void addEvent(int fd, uint32_t events) override;
    void removeEvent(int fd) override;
    void run() override;

    ~EPollLoop() override;
};
