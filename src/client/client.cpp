#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    const char* message = "PING";
    send(clientSocket, message, strlen(message) + 1, 0);

    char buffer[1024] = {0};
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived > 0) {
        cout << "Message from server: " << buffer << endl;
        if (strcmp(buffer, "PONG") == 0)
            cout << "Received PONG from server" << endl;
        else
            cout << "Did not receive PONG from server" << endl;
    } else {
        cout << "No response from server" << endl;
    }

    close(clientSocket);
    return 0;
}
