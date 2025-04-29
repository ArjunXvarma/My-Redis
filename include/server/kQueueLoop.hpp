#include "eventLoop.hpp"
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>
#include <functional>

using namespace std;

class KQueueLoop : public EventLoop {
private:
    int kq;
    int serverSocketFd;
public:
    KQueueLoop(int serverSocketFd);
    
    void addEvent(int fd, uint32_t events) override;
    void removeEvent(int fd) override;
    void run() override;

    ~KQueueLoop() override;
};
