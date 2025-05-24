#include "command/commands/hash_set_commands/hsetCmd.hpp"
#include "datastore/datastore.hpp"

std::string HSetCommand::execute(const std::vector<std::string>& args) {
    if (args.size() < 3 || args.size() % 2 != 1) {
        return "-ERR wrong number of arguments for 'hset'\r\n";
    }

    const std::string& hash_key = args[0];
    auto& ds = DataStore::getInstance();

    int addedCount = 0;

    try {
        for (size_t i = 1; i < args.size(); i += 2) {
            const std::string& field = args[i];
            const std::string& value = args[i + 1];

            try {
                bool isNewField = !ds.hexists(hash_key, field);
                ds.hset(hash_key, field, value);
                if (isNewField) {
                    ++addedCount;
                }
            } catch (const std::runtime_error&) {
                return "-ERR failed to set field in hash\r\n";
            }
        }
    } catch (const std::runtime_error& e) {
        return "-ERR " + std::string(e.what()) + "\r\n";
    }

    return ":" + std::to_string(addedCount) + "\r\n";
}