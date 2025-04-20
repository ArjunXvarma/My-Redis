#pragma once

#include "command/commandDispatcher.hpp"
#include "protocol/RESPparser.hpp"

class ClientHandler {
public:
    static bool handle(int clientSocket);
};
