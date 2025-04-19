#pragma once

#include "command/commandDispatcher.hpp"

class ClientHandler {
public:
    static bool handle(int clientSocket);
};
