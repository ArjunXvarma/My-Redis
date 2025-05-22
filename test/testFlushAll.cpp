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

    // Populate the database with key-value pairs
    cout << "Populating database with key-value pairs..." << endl;
    sendCommand(clientSocket, "*3\r\n$3\r\nSET\r\n$4\r\nkey1\r\n$6\r\nvalue1\r\n");
    sendCommand(clientSocket, "*3\r\n$3\r\nSET\r\n$4\r\nkey2\r\n$6\r\nvalue2\r\n");
    sendCommand(clientSocket, "*3\r\n$3\r\nSET\r\n$4\r\nkey3\r\n$6\r\nvalue3\r\n");

    // Verify keys are present
    cout << "Verifying keys before FLUSHALL..." << endl;
    sendCommand(clientSocket, "*1\r\n$4\r\nKEYS\r\n");

    // Use FLUSHALL command
    cout << "Testing FLUSHALL command..." << endl;
    sendCommand(clientSocket, "*1\r\n$8\r\nFLUSHALL\r\n");

    // Verify keys are deleted
    cout << "Verifying keys after FLUSHALL..." << endl;
    sendCommand(clientSocket, "*1\r\n$4\r\nKEYS\r\n");

    // Close the client socket
    close(clientSocket);
    return 0;
}