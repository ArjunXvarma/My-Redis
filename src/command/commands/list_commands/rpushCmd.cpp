#include "command/commands/list_commands/rpushCmd.hpp"
#include "datastore/datastore.hpp"

std::string RPushCommand::execute(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        return "-ERR wrong number of arguments for 'rpush'\r\n";
    }

    const std::string& key = args[0];
    std::vector<std::string> values(args.begin() + 1, args.end());
    auto& ds = DataStore::getInstance();

    try {
        ds.rpush(key, values);
        return "+OK\r\n";
    } catch (const std::runtime_error& e) {
        return "-ERR " + std::string(e.what()) + "\r\n";
    }
}