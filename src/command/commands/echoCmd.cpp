#include "command/commands/echoCmd.hpp"

string EchoCommand::execute(const vector<string>& args) {
    if (args.empty()) return "-ERR wrong number of arguments for 'echo'\r\n";
    return "$" + to_string(args[0].size()) + "\r\n" + args[0] + "\r\n";
}

