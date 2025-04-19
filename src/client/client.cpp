#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

void sendCommand(int clientSocket, const string& command) {
    send(clientSocket, command.c_str(), command.length() + 1, 0);

    char buffer[1024] = {0};
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived > 0) {
        cout << "Response: " << buffer << endl;
    } else {
        cout << "No response from server" << endl;
    }
}

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    // Test PING command
    cout << "Testing PING command..." << endl;
    sendCommand(clientSocket, "PING");

    // Test ECHO command
    cout << "Testing ECHO command..." << endl;
    sendCommand(clientSocket, "ECHO Hello, Redis!");

    // Test SET command
    cout << "Testing SET command..." << endl;
    sendCommand(clientSocket, "SET key1 value1");

    // Test GET command
    cout << "Testing GET command..." << endl;
    sendCommand(clientSocket, "GET key1");

    // Test unknown command
    cout << "Testing unknown command..." << endl;
    sendCommand(clientSocket, "UNKNOWN");

    close(clientSocket);
    return 0;
}