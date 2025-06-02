#include "command/commands/set_commands/sinterCmd.hpp"

std::string SInterCommand::execute(const std::vector<std::string>& args, TransactionContext& txn) {
    if (args.empty()) {
        return "-ERR wrong number of arguments for 'sinter'\r\n";
    }
    try {
        auto& ds = DataStore::getInstance();
        std::vector<std::string> result = ds.sinter(args);

        std::string response = "*" + std::to_string(result.size()) + "\r\n";
        for (const auto& val : result) {
            response += "$" + std::to_string(val.size()) + "\r\n" + val + "\r\n";
        }
        return response;
    } catch (const std::exception& e) {
        return "-ERR " + std::string(e.what()) + "\r\n";
    }
}