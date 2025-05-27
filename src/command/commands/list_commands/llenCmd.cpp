#include "command/commands/list_commands/llenCmd.hpp"
#include "datastore/datastore.hpp"

std::string LLenCommand::execute(const std::vector<std::string>& args) {
    if (args.size() != 1) {
        return "-ERR wrong number of arguments for 'llen'\r\n";
    }

    const std::string& key = args[0];
    auto& ds = DataStore::getInstance();

    try {
        int length = ds.llen(key);
        return ":" + std::to_string(length) + "\r\n";
    } catch (const std::runtime_error& e) {
        return "-ERR " + std::string(e.what()) + "\r\n";
    }
}