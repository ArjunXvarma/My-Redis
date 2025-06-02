#include "command/commands/list_commands/lrangeCmd.hpp"
#include "datastore/datastore.hpp"

std::string LRangeCommand::execute(const std::vector<std::string>& args, TransactionContext& txn) {
    if (args.size() != 3) {
        return "-ERR wrong number of arguments for 'lrange'\r\n";
    }

    const std::string& key = args[0];
    int start = std::stoi(args[1]);
    int end = std::stoi(args[2]);
    auto& ds = DataStore::getInstance();

    try {
        std::vector<std::string> range = ds.lrange(key, start, end);

        if (range.empty()) 
            return "*0\r\n";

        std::string response = "*" + std::to_string(range.size()) + "\r\n";
        for (const auto& value : range) {
            response += "$" + std::to_string(value.size()) + "\r\n" + value + "\r\n";
        }
        return response;
    } catch (const std::runtime_error& e) {
        return "-ERR " + std::string(e.what()) + "\r\n";
    }
}