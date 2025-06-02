#include "command/commands/list_commands/lpopCmd.hpp"
#include "datastore/datastore.hpp"

std::string LPopCommand::execute(const std::vector<std::string>& args, TransactionContext& txn) {
    if (args.size() != 2) {
        return "-ERR wrong number of arguments for 'lpop'\r\n";
    }

    const std::string& key = args[0];
    int count = std::stoi(args[1]);
    auto& ds = DataStore::getInstance();

    try {
        ds.lpop(key, count);
        return "+OK\r\n";
    } catch (const std::runtime_error& e) {
        return "-ERR " + std::string(e.what()) + "\r\n";
    }
}