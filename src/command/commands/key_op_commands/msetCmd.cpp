#include "command/commands/key_op_comamnds/msetCmd.hpp"

std::string MSetCommand::execute(const std::vector<std::string>& args) {
    if (args.empty() || args.size() & 1) {
        return "-ERR wrong number of arguments for 'mset'\r\n";
    }

    std::unordered_map<std::string, std::string> kvs;
    for (size_t i = 0; i < args.size(); i += 2) kvs[args[i]] = args[i + 1];
    
    DataStore::getInstance().mset(kvs);

    return "+OK\r\n";
}