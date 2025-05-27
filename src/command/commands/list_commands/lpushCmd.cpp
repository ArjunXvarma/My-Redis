#include "command/commands/list_commands/lpushCmd.hpp"
#include "datastore/datastore.hpp"

std::string LPushCommand::execute(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        return "-ERR wrong number of arguments for 'lpush'\r\n";
    }

    const std::string& key = args[0];
    std::vector<std::string> values(args.begin() + 1, args.end());
    auto& ds = DataStore::getInstance();

    try {
        ds.lpush(key, values);
        return "+OK\r\n";
    } catch (const std::runtime_error& e) {
        return "-ERR " + std::string(e.what()) + "\r\n";
    }
}