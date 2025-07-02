#include <thread>
#include <iostream>
#include <signal.h>
#include <fstream>
#include "server/server.hpp"

int main() {
    static std::ofstream log_file("/Users/arjunvarma/Desktop/Programs/C++/Redis/My-Redis/server.log", std::ios::out | std::ios::app);
    std::cout.rdbuf(log_file.rdbuf());
    std::cerr.rdbuf(log_file.rdbuf());
    
    signal(SIGPIPE, SIG_IGN);
    Server server;
    server.start(8080);
    return 0;
}