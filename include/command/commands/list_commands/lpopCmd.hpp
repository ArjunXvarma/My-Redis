#pragma once
#include "../../command.hpp"

class LPopCommand : public Command {
public:
    std::string execute(const std::vector<std::string>& args, TransactionContext& txn) override;
};