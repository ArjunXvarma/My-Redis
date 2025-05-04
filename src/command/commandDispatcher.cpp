#include "command/commandDispatcher.hpp"

CommandDispatcher::CommandDispatcher() {}

std::string CommandDispatcher::dispatch(const std::vector<std::string>& tokens) {
    if (tokens.empty()) return "-ERR empty command\r\n";

    ThreadPool globalThreadPool;
    std::string cmd = tokens[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
    std::vector<std::string> args(tokens.begin() + 1, tokens.end());

    if (cmd == "PING") {
        globalThreadPool.enqueue([args]() {
            return PingCommand().execute(args);
        });
    } 
    
    else if (cmd == "ECHO") {
        globalThreadPool.enqueue([args]() {
            return EchoCommand().execute(args);
        });
    } 
    
    else if (cmd == "SET") {
        globalThreadPool.enqueue([args]() {
            return SetCommand().execute(args);
        });
    } 
    
    else if (cmd == "GET") {
        globalThreadPool.enqueue([args]() {
            return GetCommand().execute(args);
        });
    } 
    
    else {
        return "-ERR unknown command '" + cmd + "'\r\n";
    }

    return "-ERR unknown command '" + cmd + "'\r\n";
}
