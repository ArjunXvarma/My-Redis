#include "command/commandDispatcher.hpp"
#include "utils/GlobalThreadPool.hpp"

CommandDispatcher::CommandDispatcher() {}

std::string CommandDispatcher::dispatch(const std::vector<std::string>& tokens) {
    if (tokens.empty()) return "-ERR empty command\r\n";

    std::string cmd = tokens[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
    std::vector<std::string> args(tokens.begin() + 1, tokens.end());
    
    auto futureResult = globalThreadPool.enqueue([cmd, args]() -> std::string {
        if (cmd == "PING") return PingCommand().execute(args);
        if (cmd == "ECHO") return EchoCommand().execute(args);
        if (cmd == "SET") return SetCommand().execute(args);
        if (cmd == "GET") return GetCommand().execute(args);

        return "-ERR unknown command '" + cmd + "'\r\n";
    });

    return futureResult.get();
}
