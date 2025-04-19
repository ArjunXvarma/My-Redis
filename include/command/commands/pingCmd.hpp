#pragma once

#include "../command.hpp"

using namespace std;

class PingCommand : public Command {
public:
    string execute(const vector<string>& args) override;
};
