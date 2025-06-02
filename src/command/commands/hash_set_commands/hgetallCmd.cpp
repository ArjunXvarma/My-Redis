#include "command/commands/hash_set_commands/hgetallCmd.hpp"
#include "datastore/datastore.hpp"

std::string HGetAllCommand::execute(const std::vector<std::string>& args, TransactionContext& txn) {
    if (args.size() != 1) {
        return "-ERR wrong number of arguments for 'hgetall'\r\n";
    }

    const std::string& hash_key = args[0];
    auto& ds = DataStore::getInstance();

    try {
        std::vector<std::string> allFields = ds.hgetall(hash_key);
        std::string response = "*" + std::to_string(allFields.size()) + "\r\n";
        for (const auto& field : allFields) {
            response += "$" + std::to_string(field.size()) + "\r\n" + field + "\r\n";
        }
        return response;
    } catch (const std::runtime_error& e) {
        return "-ERR " + std::string(e.what()) + "\r\n";
    }
}