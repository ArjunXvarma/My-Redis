#include "server/server.hpp"
#include "persistence/persistenceManager.hpp"
#include "server/kQueueLoop.hpp"
#include "server/ePollLoop.hpp"
#include <fstream>
#include <arpa/inet.h> // Required for inet_pton

std::string dumpFileName = "../dump.rdb";

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
}












// #include <iostream>
// #include <stdexcept>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <unistd.h>
// #include "networking/KqueueHandler.h"

// int createServerSocket(const std::string &ip, int port) {
//     // Create the server socket
//     int serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
//     if (serverSocketFd == -1) {
//         throw std::runtime_error("Failed to create server socket");
//     }

//     // Set socket options (e.g., reuse address)
//     int opt = 1;
//     if (setsockopt(serverSocketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
//         close(serverSocketFd);
//         throw std::runtime_error("Failed to set socket options");
//     }

//     // Bind the socket to the specified IP and port
//     sockaddr_in serverAddr{};
//     serverAddr.sin_family = AF_INET;
//     serverAddr.sin_port = htons(port);
//     if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0) {
//         close(serverSocketFd);
//         throw std::runtime_error("Invalid IP address");
//     }

//     if (bind(serverSocketFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
//         close(serverSocketFd);
//         throw std::runtime_error("Failed to bind server socket");
//     }

//     // Start listening for incoming connections
//     if (listen(serverSocketFd, 10) == -1) {
//         close(serverSocketFd);
//         throw std::runtime_error("Failed to listen on server socket");
//     }

//     return serverSocketFd;
// }

// int main() {
//     try {
//         // Create the server socket
//         int serverSocketFd = createServerSocket("127.0.0.1", 8080);
//         std::cout << "Server is running on 127.0.0.1:8080..." << std::endl;

//         // Initialize the KqueueHandler with the server socket
//         KqueueHandler kqueueHandler(serverSocketFd);

//         // Start the event loop
//         while (true) {
//             kqueueHandler.waitForEvents();
//         }

//     } catch (const std::exception &e) {
//         std::cerr << "Error: " << e.what() << std::endl;
//         return 1;
//     }

//     return 0;
// }
