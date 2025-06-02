#pragma once
#include "../../command.hpp"

class HExistsCommand : public Command {
public:
    std::string execute(const std::vector<std::string>& args, TransactionContext& txn) override;
};