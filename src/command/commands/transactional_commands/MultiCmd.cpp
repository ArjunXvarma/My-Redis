#include "command/commands/transactional_commands/MultiCmd.hpp"

std::string MultiCommand::execute(const std::vector<std::string>&, TransactionContext& txn) {
    if (txn.in_transaction) {
        return "-ERR MULTI calls cannot be nested\r\n";
    }
    txn.in_transaction = true;
    txn.queued_commands.clear();
    return "+OK\r\n";
}