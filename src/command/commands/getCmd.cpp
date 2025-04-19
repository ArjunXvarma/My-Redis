#include "command/commands/getCmd.hpp"

string GetCommand::execute(const vector<string>& args) {
    if (args.empty()) return "-ERR wrong number of arguments for 'get'\r\n";

    auto value = DataStore::getInstance().get(args[0]);
    if (!value.has_value()) return "$-1\r\n";

    return "$" + to_string(value->size()) + "\r\n" + *value + "\r\n";
}