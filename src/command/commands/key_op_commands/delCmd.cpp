#include "command/commands/key_op_comamnds/delCmd.hpp"

std::string DelCommand::execute(const std::vector<std::string>& args) {
    if (args.empty()) return "-ERR wrong number of arguments for 'del'\r\n";

    auto& ds = DataStore::getInstance(); // Get the singleton instance of the datastore
    int deletedCount = 0;

    for (const auto& key : args) {
        if (ds.del(key)) { // Assume `del` returns true if the key was deleted
            ++deletedCount;
        }
    }

    return ":" + std::to_string(deletedCount) + "\r\n";
}

