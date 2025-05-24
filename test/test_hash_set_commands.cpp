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

    // Test HSET command
    cout << "Testing HSET command..." << endl;
    sendCommand(clientSocket, "*4\r\n$4\r\nHSET\r\n$6\r\nmyhash\r\n$6\r\nfield1\r\n$6\r\nvalue1\r\n");
    sendCommand(clientSocket, "*4\r\n$4\r\nHSET\r\n$6\r\nmyhash\r\n$6\r\nfield2\r\n$6\r\nvalue2\r\n");

    // Test HGET command
    cout << "Testing HGET command..." << endl;
    sendCommand(clientSocket, "*3\r\n$4\r\nHGET\r\n$6\r\nmyhash\r\n$6\r\nfield1\r\n");

    // Test HEXISTS command
    cout << "Testing HEXISTS command..." << endl;
    sendCommand(clientSocket, "*3\r\n$7\r\nHEXISTS\r\n$6\r\nmyhash\r\n$6\r\nfield1\r\n");
    sendCommand(clientSocket, "*3\r\n$7\r\nHEXISTS\r\n$6\r\nmyhash\r\n$6\r\nfield3\r\n");

    // Test HGETALL command
    cout << "Testing HGETALL command..." << endl;
    sendCommand(clientSocket, "*2\r\n$7\r\nHGETALL\r\n$6\r\nmyhash\r\n");

    // Test HDEL command
    cout << "Testing HDEL command..." << endl;
    sendCommand(clientSocket, "*3\r\n$4\r\nHDEL\r\n$6\r\nmyhash\r\n$6\r\nfield1\r\n");

    // Verify HDEL by checking HGETALL again
    cout << "Verifying HDEL with HGETALL..." << endl;
    sendCommand(clientSocket, "*2\r\n$7\r\nHGETALL\r\n$6\r\nmyhash\r\n");

    // Close the client socket
    close(clientSocket);
    return 0;
}