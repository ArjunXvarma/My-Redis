#pragma once
#include "../command.hpp"

using namespace std;

class EchoCommand : public Command {
public:
    string execute(const vector<string>& args) override;
};
