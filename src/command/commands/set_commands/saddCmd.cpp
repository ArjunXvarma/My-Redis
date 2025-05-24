#include "command/commands/set_commands/saddCmd.hpp"
#include "datastore/datastore.hpp"

std::string SAddCommand::execute(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        return "-ERR wrong number of arguments for 'sadd'\r\n";
    }

    const std::string& key = args[0];
    std::vector<std::string> values(args.begin() + 1, args.end());
    auto& ds = DataStore::getInstance();

    try {
        int addedCount = ds.sadd(key, values);
        return ":" + std::to_string(addedCount) + "\r\n";
    } catch (const std::runtime_error& e) {
        return "-ERR " + std::string(e.what()) + "\r\n";
    }
}