#include <sstream>
#include <iostream>
#include <vector>
#include <sys/socket.h> 
#include <unistd.h>
#include <cstring>
#include "server/clientHandler.hpp"

using namespace std;

bool ClientHandler::handle(int clientSocket) {
    char buffer[1024] = {0};
    ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived <= 0) return false;

    string input(buffer);

    auto tokens = RESPParser::parse(input);
    
    if (tokens.empty()) {
        string error = RESPEncoder::encodeError("Invalid command");
        send(clientSocket, error.c_str(), error.size(), 0);
        return true;
    }

    CommandDispatcher dispatcher;
    string response = dispatcher.dispatch(tokens);
    string encoded = RESPEncoder::encodeBulkString(response); // OR encodeBulkString
    send(clientSocket, encoded.c_str(), encoded.size(), 0);

    return true;
}
