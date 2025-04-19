#pragma once

#include "server/clientHandler.hpp"
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <iostream>

class Server {
public:
    void start(int port);
};
