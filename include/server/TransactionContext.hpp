#pragma once

#include <vector>
#include <string>

struct TransactionContext {
    bool in_transaction = false;
    std::vector<std::vector<std::string>> queued_commands;
};