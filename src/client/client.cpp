#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>  // for inet_pton

using namespace std;

void sendCommand(int clientSocket, const string& command) {
    send(clientSocket, command.c_str(), command.length(), 0);

    char buffer[1024] = {0};
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

    cout << bytesReceived << " bytes received" << endl;
    if (bytesReceived > 0) cout << "Response: " << buffer << endl;
    else cout << "No response from server" << endl;
}

int main() {
    // Create the client socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        cerr << "Socket creation failed" << endl;
        return 1;
    }

    // Setup the server address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);

    // Use localhost IP address (127.0.0.1)
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) <= 0) {
        cerr << "Invalid address or Address not supported" << endl;
        return 1;
    }

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        cerr << "Failed to connect to server" << endl;
        return 1;
    }

    // Test PING command
    cout << "Testing PING command..." << endl;
    sendCommand(clientSocket, "*1\r\n$4\r\nPING\r\n");

    // Test ECHO command
    cout << "Testing ECHO command..." << endl;
    sendCommand(clientSocket, "*2\r\n$4\r\nECHO\r\n$13\r\nHello, Redis!\r\n");

    // Test SET command
    cout << "Testing SET command..." << endl;
    sendCommand(clientSocket, "*3\r\n$3\r\nSET\r\n$4\r\nkey1\r\n$6\r\nvalue1\r\n");

    // Test GET command
    cout << "Testing GET command..." << endl;
    sendCommand(clientSocket, "*2\r\n$3\r\nGET\r\n$4\r\nkey1\r\n");

    // Test unknown command
    cout << "Testing unknown command..." << endl;
    sendCommand(clientSocket, "*1\r\n$7\r\nUNKNOWN\r\n");

    // Close the client socket
    close(clientSocket);
    return 0;
}
