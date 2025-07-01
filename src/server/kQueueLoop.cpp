#include "server/kQueueLoop.hpp"

#ifdef __APPLE__

#include <memory>

KQueueLoop::KQueueLoop(int serverSocketFd) : serverSocketFd(serverSocketFd) {
    kq = kqueue();
    if (kq == -1) throw std::runtime_error("Failed to create kqueue");

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
    registered_fds.insert(fd);
}

void KQueueLoop::removeEvent(int fd) {
    if (registered_fds.find(fd) == registered_fds.end()) {
        std::cerr << "[KQueueLoop] Attempted to remove a non-existent socket: FD " << fd << std::endl;
        return;
    }
    struct kevent event;
    EV_SET(&event, fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
    if (kevent(kq, &event, 1, nullptr, 0, nullptr) == -1) {
        throw std::runtime_error("Failed to remove event");
    }
    registered_fds.erase(fd);
    clientHandlers.erase(fd);
}


void KQueueLoop::handleNewConnection() {
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(serverSocketFd, (sockaddr*)&client_addr, &client_len);
    if (client_fd == -1) {
        perror("accept");
        return;
    }
    setNonBlocking(client_fd);
    addEvent(client_fd, EVFILT_READ);
    clientHandlers[client_fd] = std::make_shared<ClientHandler>(client_fd);
    std::cout << "New client connected: FD " << client_fd << std::endl;
}

void KQueueLoop::handleClientEvent(int clientSocketFd) {
    auto it = clientHandlers.find(clientSocketFd);
    if (it == clientHandlers.end()) return;
    auto handlerPtr = it->second;

    globalThreadPool.enqueue([this, clientSocketFd, handlerPtr]() {
        try {
            std::cout << "[KQueueLoop] Handling client: FD " << clientSocketFd << std::endl;
            if (!handlerPtr->handle()) {
                removeEvent(clientSocketFd);
                extern DeadFdQueue globalDeadFdQueue;
                globalDeadFdQueue.enqueue(clientSocketFd);
                clientHandlers.erase(clientSocketFd);
                std::cout << "[KQueueLoop] client disconnected: FD " << clientSocketFd << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "[KQueueLoop] Exception in client handler: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "[KQueueLoop] Unknown exception in client handler." << std::endl;
        }
    });
}

void KQueueLoop::cleanupDeadFds() {
    extern DeadFdQueue globalDeadFdQueue;
    while (auto maybeFd = globalDeadFdQueue.try_dequeue()) {
        int fd = *maybeFd;
        std::cout << "[KQueueLoop] Cleaning up FD: " << fd << std::endl;
        removeEvent(fd);
        close(fd);
    }
}

void KQueueLoop::run() {
    try {
        struct kevent events[MAX_EVENTS];
        extern DeadFdQueue globalDeadFdQueue;

        while (true) {
            std::cout << "[KQueueLoop] Waiting for events..." << std::endl;
            int eventCount = kevent(kq, nullptr, 0, events, MAX_EVENTS, nullptr);
            if (eventCount == -1) 
                throw std::runtime_error("Failed to wait for events");
            
            std::cout << "[KQueueLoop] " << eventCount << " events occurred." << std::endl;
            for (int i = 0; i < eventCount; ++i) {
                std::cout << "[KQueueLoop] Event " << i << ": FD " << events[i].ident << ", Filter " << events[i].filter << std::endl;
                int clientSocketFd = events[i].ident;
                
                if (clientSocketFd == serverSocketFd) handleNewConnection();
                else handleClientEvent(clientSocketFd);
            }
            cleanupDeadFds();
        }
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}

KQueueLoop::~KQueueLoop() {
    close(kq);
}
#endif