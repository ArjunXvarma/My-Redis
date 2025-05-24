#include "command/commands/set_commands/smembersCmd.hpp"
#include "datastore/datastore.hpp"

std::string SMembersCommand::execute(const std::vector<std::string>& args) {
    if (args.size() != 1) {
        return "-ERR wrong number of arguments for 'smembers'\r\n";
    }

    const std::string& key = args[0];
    auto& ds = DataStore::getInstance();

    try {
        std::vector<std::string> members = ds.smembers(key);
        std::string response = "*" + std::to_string(members.size()) + "\r\n";
        for (const auto& member : members) {
            response += "$" + std::to_string(member.size()) + "\r\n" + member + "\r\n";
        }
        return response;
    } catch (const std::runtime_error& e) {
        return "-ERR " + std::string(e.what()) + "\r\n";
    }
}