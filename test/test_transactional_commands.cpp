#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

void sendCommand(int clientSocket, const string& command) {
    send(clientSocket, command.c_str(), command.length(), 0);

    char buffer[4096] = {0};
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

    cout << bytesReceived << " bytes received" << endl;
    if (bytesReceived > 0) cout << "Response:\n" << buffer << endl;
    else cout << "No response from server" << endl;
}

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        cerr << "Socket creation failed" << endl;
        return 1;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) <= 0) {
        cerr << "Invalid address or Address not supported" << endl;
        return 1;
    }

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        cerr << "Failed to connect to server" << endl;
        return 1;
    }

    // Start MULTI block
    cout << "Sending MULTI..." << endl;
    sendCommand(clientSocket, "*1\r\n$5\r\nMULTI\r\n");

    // Queue several commands
    cout << "Queueing SET commands..." << endl;
    sendCommand(clientSocket, "*3\r\n$3\r\nSET\r\n$3\r\nfoo\r\n$3\r\nbar\r\n");
    sendCommand(clientSocket, "*3\r\n$3\r\nSET\r\n$3\r\nbaz\r\n$3\r\nqux\r\n");
    sendCommand(clientSocket, "*2\r\n$3\r\nGET\r\n$3\r\nfoo\r\n");

    // Execute
    cout << "Sending EXEC..." << endl;
    sendCommand(clientSocket, "*1\r\n$4\r\nEXEC\r\n");

    // Start another MULTI block but discard it
    cout << "\nStarting another MULTI block then DISCARD..." << endl;
    sendCommand(clientSocket, "*1\r\n$5\r\nMULTI\r\n");
    sendCommand(clientSocket, "*3\r\n$3\r\nSET\r\n$4\r\nspam\r\n$5\r\negggg\r\n");

    cout << "Sending DISCARD..." << endl;
    sendCommand(clientSocket, "*1\r\n$7\r\nDISCARD\r\n");

    // Check if SET was ignored
    cout << "\nChecking if discarded key was set (should be null)..." << endl;
    sendCommand(clientSocket, "*2\r\n$3\r\nGET\r\n$4\r\nspam\r\n");

    close(clientSocket);
    return 0;
}
