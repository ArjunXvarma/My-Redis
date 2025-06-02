#include "command/commands/set_commands/sremCmd.hpp"
#include "datastore/datastore.hpp"

std::string SRemCommand::execute(const std::vector<std::string>& args, TransactionContext& txn) {
    if (args.size() < 2) {
        return "-ERR wrong number of arguments for 'srem'\r\n";
    }

    const std::string& key = args[0];
    std::vector<std::string> values(args.begin() + 1, args.end());
    auto& ds = DataStore::getInstance();

    try {
        int removedCount = ds.srem(key, values);
        return ":" + std::to_string(removedCount) + "\r\n";
    } catch (const std::runtime_error& e) {
        return "-ERR " + std::string(e.what()) + "\r\n";
    }
}