#include "command/commands/key_op_comamnds/mgetCmd.hpp"

std::string MGetCommand::execute(const std::vector<std::string>& args, TransactionContext& txn) {
    if (args.empty()) return "-ERR wrong number of arguments for 'MGET'\r\n";
    
    std::vector<std::optional<std::string>> response = DataStore::getInstance().mget(args);
    std::string resp = "*" + std::to_string(response.size()) + "\r\n";

    for (const auto& val : response) {
        if (val.has_value()) resp += "$" + std::to_string(val->size()) + "\r\n" + *val + "\r\n";
        else resp += "$-1\r\n";
    }

    return resp;
}