#include "../../command.hpp"

class MultiCommand : public Command {
public:
    std::string execute(const std::vector<std::string>& args, TransactionContext& txn) override;
};