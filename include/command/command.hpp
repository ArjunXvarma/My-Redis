#pragma once
#include <string>
#include <vector>

using namespace std;

class Command {
public:
    virtual string execute(const vector<string>& args) = 0;
    virtual ~Command() = default;
};