#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>  // for inet_pton

using namespace std;

void sendCommand(int clientSocket, const string& command) {
    send(clientSocket, command.c_str(), command.length(), 0);

    char buffer[1024] = {0};
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (bytesReceived > 0) {
        cout << "Response: " << buffer << endl;
    } else {
        cerr << "No response from server" << endl;
    }
}

string generateRandomString(size_t length) {
    const string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    string randomString;
    for (size_t i = 0; i < length; ++i) {
        randomString += characters[rand() % characters.size()];
    }
    return randomString;
}

int main() {
    srand(time(0)); // Seed the random number generator

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

    // Populate the Redis database with random key-value pairs
    int numEntries = 100; // Number of key-value pairs to insert
    for (int i = 0; i < numEntries; ++i) {
        string key = "key" + to_string(i) + "_" + generateRandomString(5);
        string value = generateRandomString(10);

        string command = "*3\r\n$3\r\nSET\r\n$" + to_string(key.size()) + "\r\n" + key + "\r\n$" +
                         to_string(value.size()) + "\r\n" + value + "\r\n";

        cout << "Sending command: " << command;
        sendCommand(clientSocket, command);
    }

    // Close the client socket
    close(clientSocket);
    return 0;
}