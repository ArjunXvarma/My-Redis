#include "command/commands/pingCmd.hpp"

std::string PingCommand::execute(const std::vector<std::string>& args) {
    if (!args.empty()) return "+" + args[0] + "\r\n";
    return "+PONG\r\n";
}

