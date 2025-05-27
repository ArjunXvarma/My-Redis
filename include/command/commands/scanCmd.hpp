#pragma once
#include "../command.hpp"

class ScanCommand : public Command {
public:
    std::string execute(const std::vector<std::string>& args) override;
};