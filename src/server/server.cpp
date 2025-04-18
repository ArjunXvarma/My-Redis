#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unordered_map>

#define PORT 8080
#define BUFFER_SIZE 1024

using namespace std;

int setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int setupServerSocket(int port) {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, 5) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    if (setNonBlocking(serverSocket) < 0) {
        perror("Non-blocking failed");
        exit(EXIT_FAILURE);
    }

    return serverSocket;
}

void handleClient(int clientSocket) {
    char buffer[BUFFER_SIZE] = {0};
    ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived <= 0) {
        close(clientSocket);
        return;
    }

    cout << "Received from client: " << buffer << endl;

    if (strcmp(buffer, "PING") == 0) {
        const char* response = "PONG";
        send(clientSocket, response, strlen(response), 0);
    } else {
        const char* response = "UNKNOWN COMMAND";
        send(clientSocket, response, strlen(response), 0);
    }
}

void runEventLoop(int serverSocket) {
    fd_set masterSet, readSet;
    FD_ZERO(&masterSet);
    FD_SET(serverSocket, &masterSet);
    int maxFd = serverSocket;

    while (true) {
        readSet = masterSet;
        int activity = select(maxFd + 1, &readSet, nullptr, nullptr, nullptr);

        if (activity < 0) {
            perror("select() failed");
            break;
        }

        for (int fd = 0; fd <= maxFd; ++fd) {
            if (FD_ISSET(fd, &readSet)) {
                if (fd == serverSocket) {
                    // Accept new client
                    int clientSocket = accept(serverSocket, nullptr, nullptr);
                    if (clientSocket < 0) {
                        perror("accept() failed");
                        continue;
                    }

                    setNonBlocking(clientSocket);
                    FD_SET(clientSocket, &masterSet);
                    if (clientSocket > maxFd) maxFd = clientSocket;

                    cout << "Accepted new connection (fd=" << clientSocket << ")" << endl;
                } else {
                    // Handle client message
                    handleClient(fd);
                    FD_CLR(fd, &masterSet);
                }
            }
        }
    }
}

int main() {
    int serverSocket = setupServerSocket(PORT);
    cout << "Server listening on port " << PORT << "..." << endl;

    runEventLoop(serverSocket);

    close(serverSocket);
    return 0;
}
