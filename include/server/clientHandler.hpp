#pragma once

#include <memory>
#include <atomic>
#include <vector>
#include <string>

#include "command/commandDispatcher.hpp"
#include "protocol/RESPparser.hpp"
#include "TransactionContext.hpp"

class ClientHandler : public std::enable_shared_from_this<ClientHandler> {
public:
    int socket_fd;
    TransactionContext txn;

    explicit ClientHandler(int fd);

    bool handle();  // Called from event loop
    bool readFromSocket(std::string& input);

    void sendError(const std::string& msg);
    void sendResponse(const std::string& response);

private:
    void dispatchToThreadPool(const std::string& input);
    void handleCommand(const std::string& input);

    std::mutex write_mutex;
    std::string writeBuffer;
};
