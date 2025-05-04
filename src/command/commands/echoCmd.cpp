#include "command/commands/echoCmd.hpp"

std::string EchoCommand::execute(const std::vector<std::string>& args) {
    if (args.empty()) return "-ERR wrong number of arguments for 'echo'\r\n";
    return "$" + std::to_string(args[0].size()) + "\r\n" + args[0] + "\r\n";
}

