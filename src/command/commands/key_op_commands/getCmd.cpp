#include "command/commands/key_op_comamnds/getCmd.hpp"

std::string GetCommand::execute(const std::vector<std::string>& args) {
    if (args.empty()) return "-ERR wrong number of arguments for 'get'\r\n";

    auto value = DataStore::getInstance().get(args[0]);
    if (!value.has_value()) return "$-1\r\n";

    return "$" + std::to_string(value->size()) + "\r\n" + *value + "\r\n";
}