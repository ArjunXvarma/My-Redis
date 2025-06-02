#pragma once
#include <string>
#include <vector>

#include "datastore/datastore.hpp"
#include "../utils/GlobalThreadPool.hpp"
#include "../server/TransactionContext.hpp"

class Command {
public:
    virtual std::string execute(const std::vector<std::string>& args, TransactionContext& txn) = 0;
    virtual ~Command() = default;
};