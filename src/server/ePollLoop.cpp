#include "server/ePollLoop.hpp"

EPollLoop::EPollLoop() {
    epollFd = epoll_create1(0);

    if (epollFd == -1) {
        perror("epoll_ctl: creation failed");
        exit(-1);
    }
}

void EPollLoop::addSocket(int fd) {
    epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = fd;

    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event) == -1) {
        perror("Adding socket to epoll has failed");
        exit(-1);
    }
}

void EPollLoop::removeSocket(int fd) {
    if (epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL) == -1) {
        perror("Removing socket to epoll has failed");
        exit(-1);
    }
}

void EPollLoop::run() {
    const int MAX_EVENTS = 1024;
    struct epoll_event events[MAX_EVENTS];

    while (true) {
        int nfds = epoll_wait(epollFd, events, MAX_EVENTS, -1);

        if (nfds == -1) {
            perror("epoll_wait");
            continue;
        }

        for (size_t i = 0; i < nfds; i++) {
            int fd = events[i].data.fd;

            cout << "Handling event" << endl;
        }
    }
}

EPollLoop::~EPollLoop() {
    close(epollFd);
}