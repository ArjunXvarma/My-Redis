#include "server/server.hpp"
#include "persistence/persistenceManager.hpp"
#include "server/kQueueLoop.hpp"
#include "server/ePollLoop.hpp"
#include <fstream>

string dumpFileName = "../dump.rdb";

int setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int setupServerSocket(int port) {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    bind(serverSocket, (sockaddr*)&addr, sizeof(addr));
    listen(serverSocket, 5);
    setNonBlocking(serverSocket);

    return serverSocket;
}

void Server::start(int port) {
    cout << "[Server] Starting server..." << endl;
    
    ofstream dumpFile(dumpFileName, ios::trunc);
    if (!dumpFile.is_open()) {
        cerr << "[Server] Failed to initialize dump.rdb file in the root directory." << endl;
        return;
    }
    cout << "[Server] Initialized dump.rdb file in the root directory." << endl;
    dumpFile.close();

    int serverSocket = setupServerSocket(port);
    cout << "Listening on port " << port << "...\n";

    // fd_set masterSet, readSet;
    // FD_ZERO(&masterSet);
    // FD_SET(serverSocket, &masterSet);
    // int maxFd = serverSocket;

    EventLoop* eventLoop;
    #ifdef __linux__
        eventLoop = new EPollLoop();
    #elif defined(__APPLE__)
        eventLoop = new KQueueLoop();
    #else  
        cerr << "[Server] Unsupported platform." << endl;
        return;
    #endif
    eventLoop->addSocket(serverSocket); 

    eventLoop->run([serverSocket](int fd) {
        if (fd == serverSocket) {
            // Accept new client connections
            int clientSocket = accept(serverSocket, nullptr, nullptr);
            if (clientSocket != -1) {
                setNonBlocking(clientSocket);
                return clientSocket; // Return the new client socket to be added to the event loop
            }
        } else {
            // Handle client requests
            if (!ClientHandler::handle(fd)) {
                close(fd);
                return -1; // Return -1 to remove the socket from the event loop
            }
        }
        return 0; // Return 0 to keep the socket in the event loop
    });

    // while (true) {
    //     readSet = masterSet;
    //     select(maxFd + 1, &readSet, nullptr, nullptr, nullptr);

    //     for (int fd = 0; fd <= maxFd; ++fd) {
    //         if (FD_ISSET(fd, &readSet)) {
    //             if (fd == serverSocket) {
    //                 int clientSocket = accept(serverSocket, nullptr, nullptr);
    //                 setNonBlocking(clientSocket);
    //                 FD_SET(clientSocket, &masterSet);
    //                 maxFd = max(maxFd, clientSocket);
    //             } else {
    //                 if (!ClientHandler::handle(fd)) {
    //                     close(fd);
    //                     FD_CLR(fd, &masterSet);
    //                 }
    //             }
    //         }
    //     }
    // }
}
