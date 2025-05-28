#pragma once

#include "../../command.hpp"
#include "datastore/datastore.hpp"
#include <unordered_map>

class SetCommand : public Command {
public:
    std::string execute(const std::vector<std::string>& args) override;
};
