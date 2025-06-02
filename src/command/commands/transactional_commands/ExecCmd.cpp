#include "command/commands/transactional_commands/ExecCmd.hpp"
#include "command/commandDispatcher.hpp"

std::string ExecCommand::execute(const std::vector<std::string>&, TransactionContext& txn) {
    if (!txn.in_transaction) {
        return "-ERR EXEC without MULTI\r\n";
    }

    txn.in_transaction = false;
    if (txn.queued_commands.empty()) {
        return "*0\r\n";
    }

    std::string response = "*" + std::to_string(txn.queued_commands.size()) + "\r\n";
    CommandDispatcher dispatcher;

    for (const auto& tokens : txn.queued_commands) {
        std::string result = dispatcher.dispatch(tokens, txn); // txn.in_transaction is false
        response += result;
    }

    txn.queued_commands.clear();
    return response;
}