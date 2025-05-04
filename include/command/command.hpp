#pragma once
#include <string>
#include <vector>

class Command {
public:
    virtual std::string execute(const std::vector<std::string>& args) = 0;
    virtual ~Command() = default;
};