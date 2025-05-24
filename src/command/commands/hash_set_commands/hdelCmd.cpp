#include "command/commands/hash_set_commands/hdelCmd.hpp"
#include "datastore/datastore.hpp"

std::string HDelCommand::execute(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        return "-ERR wrong number of arguments for 'HDEL'\r\n";
    }

    const std::string& hash_key = args[0];
    const std::vector<std::string> fields(args.begin() + 1, args.end());

    auto& ds = DataStore::getInstance();
    int deletedCount = 0;

    try {
        for (const auto& field : fields) {
            try {
                ds.hdel(hash_key, field);
                ++deletedCount;
            } catch (const std::runtime_error&) {
                // Ignore errors for non-existent fields
            }
        }
    } catch (const std::runtime_error& e) {
        return "-ERR " + std::string(e.what()) + "\r\n";
    }

    return ":" + std::to_string(deletedCount) + "\r\n";
}