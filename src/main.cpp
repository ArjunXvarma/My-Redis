#include <thread>
#include <iostream>
#include "server/server.hpp"

int main() {
    Server server;
    server.start(8080);
    return 0;
}