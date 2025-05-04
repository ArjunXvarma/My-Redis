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
#include "../utils/GlobalThreadPool.hpp"

class CommandDispatcher {
public:
    CommandDispatcher();
    std::string dispatch(const std::vector<std::string>& args);

private:
    std::unordered_map<std::string, std::unique_ptr<Command>> commandMap;
};
