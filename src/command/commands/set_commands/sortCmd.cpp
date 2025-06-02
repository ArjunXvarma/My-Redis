#include "command/commands/set_commands/sortCmd.hpp"

std::string SortCommand::execute(const std::vector<std::string>& args, TransactionContext& txn) {
    if (args.empty()) {
        return "-ERR wrong number of arguments for 'sort'\r\n";
    }

    const std::string& key = args[0];
    std::string order = "ASC";
    int offset = 0;
    int count = -1;

    // Optional arguments: order, offset, count
    if (args.size() > 1) order = args[1];
    if (args.size() > 2) offset = std::stoi(args[2]);
    if (args.size() > 3) count = std::stoi(args[3]);

    try {
        auto& ds = DataStore::getInstance();
        std::vector<std::string> sorted = ds.sort(key, order, offset, count);

        std::string response = "*" + std::to_string(sorted.size()) + "\r\n";
        for (const auto& val : sorted) {
            response += "$" + std::to_string(val.size()) + "\r\n" + val + "\r\n";
        }
        return response;
    } 
    
    catch (const std::exception& e) {
        return "-ERR " + std::string(e.what()) + "\r\n";
    }
}