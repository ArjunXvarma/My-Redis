#pragma once
#include "../command.hpp"

class SortCommand : public Command {
public:
    std::string execute(const std::vector<std::string>& args) override;
};