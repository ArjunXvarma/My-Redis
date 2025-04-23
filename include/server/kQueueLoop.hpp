#include "eventLoop.hpp"
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>

using namespace std;

class KQueueLoop : EventLoop {
private:
    int kq;
public:
    KQueueLoop();
    
    void addSocket(int fd) override;
    void removeSocket(int fd) override;
    void run() override;

    ~KQueueLoop() override;
};
