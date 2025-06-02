#include "command/commands/transactional_commands/DiscardCmd.hpp"

std::string DiscardCommand::execute(const std::vector<std::string>&, TransactionContext& txn) {
    if (!txn.in_transaction) {
        return "-ERR DISCARD without MULTI\r\n";
    }
    txn.in_transaction = false;
    txn.queued_commands.clear();
    return "+OK\r\n";
}