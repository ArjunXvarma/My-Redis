#include "command/commands/set_commands/sismemberCmd.hpp"
#include "datastore/datastore.hpp"

std::string SIsMemberCommand::execute(const std::vector<std::string>& args) {
    if (args.size() != 2) {
        return "-ERR wrong number of arguments for 'sismember'\r\n";
    }

    const std::string& key = args[0];
    const std::string& value = args[1];
    auto& ds = DataStore::getInstance();

    try {
        bool isMember = ds.sismember(key, value);
        return ":" + std::to_string(isMember ? 1 : 0) + "\r\n";
    } catch (const std::runtime_error& e) {
        return "-ERR " + std::string(e.what()) + "\r\n";
    }
}