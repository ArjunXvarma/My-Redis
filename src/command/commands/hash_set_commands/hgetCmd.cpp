#include "command/commands/hash_set_commands/hgetCmd.hpp"
#include "datastore/datastore.hpp"

std::string HGetCommand::execute(const std::vector<std::string>& args) {
    if (args.size() != 2) {
        return "-ERR wrong number of arguments for 'hget'\r\n";
    }

    const std::string& hash_key = args[0];
    const std::string& field = args[1];

    auto& ds = DataStore::getInstance();

    try {
        std::string value = ds.hget(hash_key, field);
        return "$" + std::to_string(value.size()) + "\r\n" + value + "\r\n";
    } catch (const std::runtime_error& e) {
        return "-ERR " + std::string(e.what()) + "\r\n";
    }
}