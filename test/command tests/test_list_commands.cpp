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

    // Test LPUSH command
    cout << "Testing LPUSH command..." << endl;
    sendCommand(clientSocket, "*3\r\n$5\r\nLPUSH\r\n$6\r\nmylist\r\n$6\r\nvalue1\r\n");
    sendCommand(clientSocket, "*3\r\n$5\r\nLPUSH\r\n$6\r\nmylist\r\n$6\r\nvalue2\r\n");

    // Test RPUSH command
    cout << "Testing RPUSH command..." << endl;
    sendCommand(clientSocket, "*3\r\n$5\r\nRPUSH\r\n$6\r\nmylist\r\n$6\r\nvalue3\r\n");
    sendCommand(clientSocket, "*3\r\n$5\r\nRPUSH\r\n$6\r\nmylist\r\n$6\r\nvalue4\r\n");

    // Test LLEN command
    cout << "Testing LLEN command..." << endl;
    sendCommand(clientSocket, "*2\r\n$4\r\nLLEN\r\n$6\r\nmylist\r\n");

    // Test LRANGE command
    cout << "Testing LRANGE command..." << endl;
    sendCommand(clientSocket, "*4\r\n$6\r\nLRANGE\r\n$6\r\nmylist\r\n$1\r\n0\r\n$1\r\n3\r\n");

    // Test LPOP command
    cout << "Testing LPOP command..." << endl;
    sendCommand(clientSocket, "*3\r\n$4\r\nLPOP\r\n$6\r\nmylist\r\n$1\r\n2\r\n");

    // Test RPOP command
    cout << "Testing RPOP command..." << endl;
    sendCommand(clientSocket, "*3\r\n$4\r\nRPOP\r\n$6\r\nmylist\r\n$1\r\n2\r\n");

    // Verify the list with LRANGE
    cout << "Verifying list with LRANGE after pops..." << endl;
    sendCommand(clientSocket, "*4\r\n$6\r\nLRANGE\r\n$6\r\nmylist\r\n$1\r\n0\r\n$2\r\n-1\r\n");

    // Close the client socket
    close(clientSocket);
    return 0;
}