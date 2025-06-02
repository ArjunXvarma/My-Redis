#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>

#include "command.hpp"
#include "allCommands.hpp"
#include "../protocol/RESPencoder.hpp"
#include "../utils/GlobalThreadPool.hpp"
#include "../server/TransactionContext.hpp"

class CommandDispatcher {
public:
    CommandDispatcher();
    std::string dispatch(const std::vector<std::string>& tokens, TransactionContext& txn);
    std::string executeCommand(const std::string& cmd, const std::vector<std::string>& args, TransactionContext& txn);
    std::string handleTransactionCommand(const std::string& cmd, TransactionContext& txn);
    
private:
    std::unordered_map<std::string, std::unique_ptr<Command>> commandMap;
};
