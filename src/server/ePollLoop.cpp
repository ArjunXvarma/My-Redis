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
        perror("epoll_ctl: add");
        return;
    }

    activeSockets.insert(fd);
}

void EPollLoop::removeEvent(int fd) {
    if (activeSockets.find(fd) == activeSockets.end()) {
        std::cerr << "[EPollLoop] Attempted to remove a non-existent socket: FD " << fd << std::endl;
        return;
    }

    if (epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, nullptr) == -1) {
        perror("epoll_ctl: remove");
        return;
    }

    clientHandlers.erase(fd);
    activeSockets.erase(fd);
}

void EPollLoop::run() {
    struct epoll_event events[MAX_EVENTS];
    extern DeadFdQueue globalDeadFdQueue;

    while (true) {
        size_t nfds = epoll_wait(epollFd, events, MAX_EVENTS, -1);

        if ((int)nfds == -1) {
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
                clientHandlers.emplace(client_fd, ClientHandler(client_fd));

                std::cout << "New client connected: FD " << client_fd << std::endl;
            }

            else {
                globalThreadPool.enqueue([this, fd]() {
                    std::cout << "[EPollLoop] Handling client: FD " << fd << std::endl;
                    auto it = clientHandlers.find(fd);
                    if (it == clientHandlers.end()) return;

                    ClientHandler& handler = it->second;
                    if (!handler.handle()) {
                        removeEvent(fd);
                        globalDeadFdQueue.enqueue(fd);
                        clientHandlers.erase(fd);  // Cleanup
                        std::cout << "[EPollLoop] Client disconnected: FD " << fd << std::endl;
                    }
                });
            }
        }

        while (auto maybeFd = globalDeadFdQueue.try_dequeue()) {
            int fd = *maybeFd;
            std::cout << "[EPollLoop] Cleaning up FD: " << fd << std::endl;
            removeEvent(fd);
            close(fd);
            clientHandlers.erase(fd);  // <- make sure you remove the ClientHandler
        }
    }
}

EPollLoop::~EPollLoop() {
    close(epollFd);
}

#endif