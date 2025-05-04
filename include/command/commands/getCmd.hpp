#pragma once

#include "datastore/datastore.hpp"
#include "../command.hpp"

class GetCommand : public Command {
public:
    std::string execute(const std::vector<std::string>& args) override;
};