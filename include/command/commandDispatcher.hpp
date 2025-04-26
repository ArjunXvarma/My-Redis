#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>

#include "command.hpp"
#include "commands/getCmd.hpp"
#include "commands/setCmd.hpp"
#include "commands/pingCmd.hpp"
#include "commands/echoCmd.hpp"
#include "../protocol/RESPencoder.hpp"

using namespace std;

class CommandDispatcher {
public:
    CommandDispatcher();
    string dispatch(const vector<string>& args);

private:
    unordered_map<string, unique_ptr<Command>> commandMap;
};
