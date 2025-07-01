#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h> 

using namespace std;

void sendCommand(int clientSocket, const string& command) {
    send(clientSocket, command.c_str(), command.length(), 0);

    char buffer[2048] = {0};
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

    // Test SADD command
    cout << "Testing SADD command..." << endl;
    sendCommand(clientSocket, "*3\r\n$4\r\nSADD\r\n$4\r\nset1\r\n$6\r\nvalue1\r\n");
    sendCommand(clientSocket, "*3\r\n$4\r\nSADD\r\n$4\r\nset1\r\n$6\r\nvalue2\r\n");
    sendCommand(clientSocket, "*3\r\n$4\r\nSADD\r\n$4\r\nset1\r\n$6\r\nvalue3\r\n");

    // Test SMEMBERS command
    cout << "Testing SMEMBERS command..." << endl;
    sendCommand(clientSocket, "*2\r\n$8\r\nSMEMBERS\r\n$4\r\nset1\r\n");

    // Test SISMEMBER command
    cout << "Testing SISMEMBER command..." << endl;
    sendCommand(clientSocket, "*3\r\n$9\r\nSISMEMBER\r\n$4\r\nset1\r\n$6\r\nvalue1\r\n");
    sendCommand(clientSocket, "*3\r\n$9\r\nSISMEMBER\r\n$4\r\nset1\r\n$6\r\nvalue4\r\n");

    // Test SREM command
    cout << "Testing SREM command..." << endl;
    sendCommand(clientSocket, "*3\r\n$4\r\nSREM\r\n$4\r\nset1\r\n$6\r\nvalue1\r\n");
    sendCommand(clientSocket, "*3\r\n$4\r\nSREM\r\n$4\r\nset1\r\n$6\r\nvalue2\r\n");

    // Verify SREM by checking SMEMBERS again
    cout << "Verifying SREM with SMEMBERS..." << endl;
    sendCommand(clientSocket, "*2\r\n$8\r\nSMEMBERS\r\n$4\r\nset1\r\n");

    // --- Additional tests for SINTER, SUNION, and SORT ---

    // Prepare more sets for SINTER and SUNION
    cout << "Adding members to set2 and set3..." << endl;
    sendCommand(clientSocket, "*4\r\n$4\r\nSADD\r\n$4\r\nset2\r\n$6\r\nvalue2\r\n$6\r\nvalue3\r\n");
    sendCommand(clientSocket, "*4\r\n$4\r\nSADD\r\n$4\r\nset3\r\n$6\r\nvalue3\r\n$6\r\nvalue4\r\n");

    // Test SINTER
    cout << "Testing SINTER command..." << endl;
    sendCommand(clientSocket, "*4\r\n$6\r\nSINTER\r\n$4\r\nset1\r\n$4\r\nset2\r\n$4\r\nset3\r\n");

    // Test SUNION
    cout << "Testing SUNION command..." << endl;
    sendCommand(clientSocket, "*4\r\n$6\r\nSUNION\r\n$4\r\nset1\r\n$4\r\nset2\r\n$4\r\nset3\r\n");

    // Prepare a list for SORT
    cout << "Adding members to list for SORT..." << endl;
    sendCommand(clientSocket, "*4\r\n$5\r\nLPUSH\r\n$5\r\nalist\r\n$1\r\n3\r\n$1\r\n1\r\n");
    sendCommand(clientSocket, "*3\r\n$5\r\nRPUSH\r\n$5\r\nalist\r\n$1\r\n2\r\n");

    // Test SORT ASC
    cout << "Testing SORT ASC..." << endl;
    sendCommand(clientSocket, "*3\r\n$4\r\nSORT\r\n$5\r\nalist\r\n$3\r\nASC\r\n");

    // Test SORT DESC
    cout << "Testing SORT DESC..." << endl;
    sendCommand(clientSocket, "*3\r\n$4\r\nSORT\r\n$5\r\nalist\r\n$4\r\nDESC\r\n");

    // Test SORT with offset and count
    cout << "Testing SORT ASC with offset=1, count=1..." << endl;
    sendCommand(clientSocket, "*5\r\n$4\r\nSORT\r\n$5\r\nalist\r\n$3\r\nASC\r\n$1\r\n1\r\n$1\r\n1\r\n");

    // Close the client socket
    close(clientSocket);
    return 0;
}