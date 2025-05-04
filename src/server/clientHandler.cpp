#include <sstream>
#include <iostream>
#include <vector>
#include <sys/socket.h> 
#include <unistd.h>
#include <cstring>
#include "server/clientHandler.hpp"

bool ClientHandler::handle(int clientSocket) {
    char buffer[1024] = {0};
    ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0); // -1 for null terminator

    if (bytesReceived == 0) {
        std::cout << "Client disconnected." << std::endl;
        return false; // socket closed by client
    } 
    
    else if (bytesReceived < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) return true;
        
        else {
            perror("recv");
            return false; // genuine error
        }
    }

    buffer[bytesReceived] = '\0';
    std::string input(buffer);

    auto tokens = RESPParser::parse(input);

    if (tokens.empty()) {
        std::string error = RESPEncoder::encodeError("Invalid or incomplete command");
        send(clientSocket, error.c_str(), error.size(), 0);
        return true;
    }

    CommandDispatcher dispatcher;
    std::string response = dispatcher.dispatch(tokens);
    std::string encoded = RESPEncoder::encodeBulkString(response);

    size_t totalSent = 0;
    while (totalSent < encoded.size()) {
        ssize_t sent = send(clientSocket, encoded.c_str() + totalSent, encoded.size() - totalSent, 0);
        if (sent <= 0) {
            perror("send failed");
            return false;
        }
        totalSent += sent;
    }

    return true;
}
