#pragma once

#include "command/commandDispatcher.hpp"
#include "protocol/RESPparser.hpp"
#include "TransactionContext.hpp"

class ClientHandler {
public:
    int socket_fd;
    TransactionContext txn;
    
    ClientHandler(int fd) : socket_fd(fd) {}
    
    bool handle();
    void sendError(const std::string& msg);
    void sendResponse(const std::string& response);
    std::vector<std::string> parseInput(const std::string& input);
    std::string processCommand(const std::vector<std::string>& tokens);
    bool readFromSocket(std::string& input);
};
