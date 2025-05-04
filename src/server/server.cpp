#include "server/server.hpp"
#include "persistence/persistenceManager.hpp"
#include "server/kQueueLoop.hpp"
#include "server/ePollLoop.hpp"
#include <fstream>
#include <arpa/inet.h> 
#include <utils/DeadFdQueue.hpp>

std::string dumpFileName = "../dump.rdb";
DeadFdQueue globalDeadFdQueue;

int setupServerSocket(int port) {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("socket");
        exit(1);
    }

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    addr.sin_port = htons(port);

    if (::bind(serverSocket, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("[Server] bind failed");
        exit(1);
    }

    if (listen(serverSocket, 5) < 0) {
        perror("[Server] listen failed");
        exit(1);
    }

    return serverSocket;
}

void Server::start(int port) {
    std::cout << "[Server] Starting server..." << std::endl;
    
    std::ofstream dumpFile(dumpFileName, std::ios::trunc);
    if (!dumpFile.is_open()) {
        std::cerr << "[Server] Failed to initialize dump.rdb file in the root directory." << std::endl;
        return;
    }
    std::cout << "[Server] Initialized dump.rdb file in the root directory." << std::endl;
    dumpFile.close();

    int serverSocket = setupServerSocket(port);
    std::cout << "Listening on port " << port << "...\n";

    EventLoop* eventLoop;
    #ifdef __linux__
        eventLoop = new EPollLoop(serverSocket);
    #elif defined(__APPLE__)
        eventLoop = new KQueueLoop(serverSocket);
    #else  
        std::cerr << "[Server] Unsupported platform." << std::endl;
        return;
    #endif

    eventLoop->run();
    
    std::cout << "[Server] Server stopped." << std::endl;
    delete eventLoop;
    close(serverSocket);
}