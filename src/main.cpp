#include <thread>
#include <iostream>
#include <signal.h>
#include "server/server.hpp"

int main() {
    signal(SIGPIPE, SIG_IGN);
    Server server;
    server.start(8080);
    return 0;
}