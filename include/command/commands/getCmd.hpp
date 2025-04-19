#pragma once

#include "datastore/datastore.hpp"
#include "../command.hpp"

using namespace std;

class GetCommand : public Command {
public:
    string execute(const vector<string>& args) override;
};