#pragma once
#include "../command.hpp"

class EchoCommand : public Command {
public:
    std::string execute(const std::vector<std::string>& args) override;
};
