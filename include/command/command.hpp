#pragma once
#include <string>
#include <vector>

#include "datastore/datastore.hpp"
#include "../utils/GlobalThreadPool.hpp"

class Command {
public:
    virtual std::string execute(const std::vector<std::string>& args) = 0;
    virtual ~Command() = default;
};