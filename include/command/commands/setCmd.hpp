#pragma once

#include "../command.hpp"
#include "datastore/datastore.hpp"
#include <unordered_map>

using namespace std;

class SetCommand : public Command {
public:
    string execute(const vector<string>& args) override;
};
