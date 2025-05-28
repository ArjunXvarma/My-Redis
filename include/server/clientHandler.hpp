#pragma once

#include "command/commandDispatcher.hpp"
#include "protocol/RESPparser.hpp"

struct TransactionContext {
    bool in_transaction = false;
    std::vector<std::string> queued_commands;
};

class ClientHandler {
public:
    int socket_fd;
    TransactionContext txn;
    ClientHandler(int fd) : socket_fd(fd) {}
    bool handle();
};
