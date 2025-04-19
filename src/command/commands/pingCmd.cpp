#include "command/commands/pingCmd.hpp"

string PingCommand::execute(const vector<string>& args) {
    if (!args.empty()) return "+" + args[0] + "\r\n";
    return "+PONG\r\n";
}

