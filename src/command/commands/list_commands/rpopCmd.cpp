#include "command/commands/list_commands/rpopCmd.hpp"
#include "datastore/datastore.hpp"

std::string RPopCommand::execute(const std::vector<std::string>& args) {
    if (args.size() != 2) {
        return "-ERR wrong number of arguments for 'rpop'\r\n";
    }

    const std::string& key = args[0];
    int count = std::stoi(args[1]);
    auto& ds = DataStore::getInstance();

    try {
        ds.rpop(key, count);
        return "+OK\r\n";
    } catch (const std::runtime_error& e) {
        return "-ERR " + std::string(e.what()) + "\r\n";
    }
}