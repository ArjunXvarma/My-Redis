#include "command/commands/key_op_comamnds/flushallCmd.hpp"

std::string FlushAllCommand::execute(const std::vector<std::string>& args) {
    if (args.size() > 1) return "-ERR wrong number of arguments for 'flushall'\r\n";

    auto& ds = DataStore::getInstance();

    bool isAsync = !args.empty() && args[0] == "ASYNC";

    if (isAsync) {
        globalThreadPool.enqueue([&ds]() {
            ds.flushAll();
        });
        return "+OK (async)\r\n";
    } 
    
    else {
        ds.flushAll();
        return "+OK\r\n";
    }
}