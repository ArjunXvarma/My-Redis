#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

using namespace std;

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress));

    listen(serverSocket, 5);
    cout << "Listening on " << PORT << endl;

    while (true) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);

        char buffer[1024] = { 0 };
        recv(clientSocket, buffer, sizeof(buffer), 0);
        cout << "Message from client: " << buffer << endl;
        if (buffer == "PING") 
            send(clientSocket, "PONG", sizeof("PONG"), 0);
        
        else break;
    }

    close(serverSocket);

    return 0;
}