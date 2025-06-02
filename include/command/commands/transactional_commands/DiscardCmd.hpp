#include "../../command.hpp"

class DiscardCommand : public Command {
public:
    std::string execute(const std::vector<std::string>& args, TransactionContext& txn) override;
};