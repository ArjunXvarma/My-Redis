#include "server/server.hpp"

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
    int serverSocket = setupServerSocket(port);
    cout << "Listening on port " << port << "...\n";

    fd_set masterSet, readSet;
    FD_ZERO(&masterSet);
    FD_SET(serverSocket, &masterSet);
    int maxFd = serverSocket;

    while (true) {
        readSet = masterSet;
        select(maxFd + 1, &readSet, nullptr, nullptr, nullptr);

        for (int fd = 0; fd <= maxFd; ++fd) {
            if (FD_ISSET(fd, &readSet)) {
                if (fd == serverSocket) {
                    int clientSocket = accept(serverSocket, nullptr, nullptr);
                    setNonBlocking(clientSocket);
                    FD_SET(clientSocket, &masterSet);
                    maxFd = max(maxFd, clientSocket);
                } else {
                    if (!ClientHandler::handle(fd)) {
                        close(fd);
                        FD_CLR(fd, &masterSet);
                    }
                }
            }
        }
    }
}
