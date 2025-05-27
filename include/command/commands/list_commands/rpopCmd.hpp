#pragma once
#include "../../command.hpp"

class RPopCommand : public Command {
public:
    std::string execute(const std::vector<std::string>& args) override;
};