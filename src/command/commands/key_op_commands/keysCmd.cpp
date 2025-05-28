#include "command/commands/key_op_comamnds/keysCmd.hpp"

std::string KeysCommand::execute(const std::vector<std::string>& args) {
    if (!args.empty()) return "-ERR wrong number of arguments for 'keys'\r\n";

    auto& ds = DataStore::getInstance();

    std::vector<std::string> keys = ds.allKeys();

    std::string response = "*" + std::to_string(keys.size()) + "\r\n";
    for (const auto& key : keys) {
        response += "$" + std::to_string(key.size()) + "\r\n" + key + "\r\n";
    }

    return response;
}