#include "server/kQueueLoop.hpp"

#ifdef __APPLE__

ThreadPool globalThreadPool;

KQueueLoop::KQueueLoop(int serverSocketFd) : serverSocketFd(serverSocketFd) {
    kq = kqueue();
    if (kq == -1) throw std::runtime_error("Failed to create kqueue");

    // Add the server socket to the kqueue
    setNonBlocking(serverSocketFd);
    addEvent(serverSocketFd, EVFILT_READ);
}

void KQueueLoop::addEvent(int fd, uint32_t events) {
    struct kevent event;
    EV_SET(&event, fd, events, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, nullptr);
    if (kevent(kq, &event, 1, nullptr, 0, nullptr) == -1) {
        perror("kevent addEvent");
        throw std::runtime_error("Failed to add event");
    }
}

void KQueueLoop::removeEvent(int fd) {
    struct kevent event;
    EV_SET(&event, fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
    if (kevent(kq, &event, 1, nullptr, 0, nullptr) == -1) {
        throw std::runtime_error("Failed to remove event");
    }
}

void KQueueLoop::run() {
    struct kevent events[MAX_EVENTS];
    extern DeadFdQueue globalDeadFdQueue;

    while (true) {
        std::cout << "[KQueueLoop] Waiting for events..." << std::endl;
        int eventCount = kevent(kq, nullptr, 0, events, MAX_EVENTS, nullptr);
        if (eventCount == -1) {
            throw std::runtime_error("Failed to wait for events");
        }
        
        std::cout << "[KQueueLoop] " << eventCount << " events occurred." << std::endl;
        for (int i = 0; i < eventCount; ++i) {
            std::cout << "[KQueueLoop] Event " << i << ": FD " << events[i].ident << ", Filter " << events[i].filter << std::endl;
            if (events[i].ident == serverSocketFd) {
                // Accept new connection
                sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(serverSocketFd, (sockaddr*)&client_addr, &client_len);
                if (client_fd == -1) {
                    perror("accept");
                    continue;
                }
    
                setNonBlocking(client_fd);
                addEvent(client_fd, EVFILT_READ);
    
                std::cout << "New client connected: FD " << client_fd << std::endl;
            } 
            
            else {
                int clientSocketFd = events[i].ident;
                
                globalThreadPool.enqueue(std::function<void()> ([this, clientSocketFd]() {
                    std::cout << "[KQueueLoop] Handling client: FD " << clientSocketFd << std::endl;
                    if (!ClientHandler::handle(clientSocketFd)) {
                        removeEvent(clientSocketFd);
                        globalDeadFdQueue.enqueue(clientSocketFd);
                        std::cout << "[KQueueLoop] Client disconnected: FD " << clientSocketFd << std::endl;
                    }
                }));
            }
        }

        while (auto maybeFd = globalDeadFdQueue.try_dequeue()) {
            int fd = *maybeFd;
            std::cout << "[KQueueLoop] Cleaning up FD: " << fd << std::endl;
            removeEvent(fd);
            close(fd);
        }
    }
}

KQueueLoop::~KQueueLoop() {
    close(kq);
}
#endif