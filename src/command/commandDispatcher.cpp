#include "command/commandDispatcher.hpp"

CommandDispatcher::CommandDispatcher() {}

string CommandDispatcher::dispatch(const vector<string>& tokens) {
    if (tokens.empty()) return "-ERR empty command\r\n";

    // TODO: Add RESP protocol support
    string cmd = tokens[0];
    transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
    vector<string> args(tokens.begin() + 1, tokens.end());

    if (cmd == "PING") return PingCommand().execute(args);
    if (cmd == "ECHO") return EchoCommand().execute(args);
    if (cmd == "SET") return SetCommand().execute(args);
    if (cmd == "GET") return GetCommand().execute(args);

    return "-ERR unknown command '" + cmd + "'\r\n";
}
