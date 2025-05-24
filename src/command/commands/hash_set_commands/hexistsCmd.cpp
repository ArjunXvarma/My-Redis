#include "command/commands/hash_set_commands/hexistsCmd.hpp"

std::string HExistsCommand::execute(const std::vector<std::string>& args) {
    if (args.size() != 2) 
        return "-ERR wrong number of arguments for 'hexists'\r\n";

    const std::string& hash_key = args[0];
    const std::string& field = args[1];

    auto& ds = DataStore::getInstance();

    try {
        bool exists = ds.hexists(hash_key, field);
        return ":" + std::to_string(exists ? 1 : 0) + "\r\n";
    } catch (const std::runtime_error& e) {
        return "-ERR " + std::string(e.what()) + "\r\n";
    }
}