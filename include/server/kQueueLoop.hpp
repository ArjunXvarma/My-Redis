#include "eventLoop.hpp"
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>
#include <functional>

using namespace std;

class KQueueLoop : public EventLoop {
private:
    int kq;
public:
    KQueueLoop();
    
    void addSocket(int fd) override;
    void removeSocket(int fd) override;
    void run(function<int(int)> eventHandler) override;

    ~KQueueLoop() override;
};
