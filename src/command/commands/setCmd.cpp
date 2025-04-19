#include "command/commands/setCmd.hpp"

string SetCommand::execute(const vector<string>& args) {
    if (args.size() < 2) return "-ERR wrong number of arguments for 'set'\r\n";

    DataStore::getInstance().set(args[0], args[1]);
    return "+OK\r\n";
}

