#include "server/ePollLoop.hpp"

#ifdef __linux__
EPollLoop::EPollLoop(int serverSocketFd) 
    : serverSocketFd(serverSocketFd), epollFd(epoll_create1(0)) {
    if (epollFd == -1) {
        perror("epoll_ctl: creation failed");
        exit(-1);
    }

    addEvent(serverSocketFd, EPOLLIN);
}

void EPollLoop::addEvent(int fd, uint32_t events) {
    epoll_event event;
    event.events = events;
    event.data.fd = fd;

    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event) == -1) {
        perror("Adding socket to epoll has failed");
        exit(-1);
    }
}

void EPollLoop::removeEvent(int fd) {
    if (epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL) == -1) {
        perror("Removing socket to epoll has failed");
        exit(-1);
    }
}

void EPollLoop::run() {
    struct epoll_event events[MAX_EVENTS];

    while (true) {
        int nfds = epoll_wait(epollFd, events, MAX_EVENTS, -1);

        if (nfds == -1) {
            perror("epoll_wait");
            continue;
        }

        for (size_t i = 0; i < nfds; i++) {
            int fd = events[i].data.fd;

            if (fd == serverSocketFd) {
                // Accept new connection
                sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(serverSocketFd, (sockaddr*)&client_addr, &client_len);
                if (client_fd == -1) {
                    perror("accept");
                    continue;
                }

                setNonBlocking(client_fd);
                addEvent(client_fd, EPOLLIN);

                std::cout << "New client connected: FD " << client_fd << std::endl;
            }

            else {
                // Handle client message
                if (!ClientHandler::handle(fd)) {
                    removeEvent(fd);
                    close(fd);
                }
            }
        }
    }
}

EPollLoop::~EPollLoop() {
    
}

#endif