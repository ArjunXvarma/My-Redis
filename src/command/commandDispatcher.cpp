#include "command/commandDispatcher.hpp"
#include "utils/GlobalThreadPool.hpp"

#ifdef __linux__
ThreadPool globalThreadPool;
#endif

bool isTransactionCommand(const std::string& cmd) {
    return cmd == "MULTI" || cmd == "EXEC" || cmd == "DISCARD";
}

CommandDispatcher::CommandDispatcher() {}

std::string CommandDispatcher::executeCommand(const std::string& cmd, const std::vector<std::string>& args, TransactionContext& txn) {
    // Normalize command to uppercase for matching
    std::string cmd_upper = cmd;
    std::transform(cmd_upper.begin(), cmd_upper.end(), cmd_upper.begin(), ::toupper);

    // Define heavy commands
    static const std::unordered_set<std::string> heavyCommands = {
        "SINTER", "SUNION", "SORT", "FLUSHALL",
        "MSET", "MGET",
        "LRANGE", "RPUSH", "LPUSH",
        "HGETALL"
    };

    // Lambda to handle all command logic
    auto task = [cmd_upper, args, &txn]() -> std::string {
        if (cmd_upper == "PING") return PingCommand().execute(args, txn);
        if (cmd_upper == "ECHO") return EchoCommand().execute(args, txn);
        if (cmd_upper == "SET") return SetCommand().execute(args, txn);
        if (cmd_upper == "GET") return GetCommand().execute(args, txn);
        if (cmd_upper == "DEL") return DelCommand().execute(args, txn);
        if (cmd_upper == "FLUSHALL") return FlushAllCommand().execute(args, txn);
        if (cmd_upper == "KEYS") return KeysCommand().execute(args, txn);
        if (cmd_upper == "MGET") return MGetCommand().execute(args, txn);
        if (cmd_upper == "MSET") return MSetCommand().execute(args, txn);
        if (cmd_upper == "SINTER") return SInterCommand().execute(args, txn);
        if (cmd_upper == "SORT") return SortCommand().execute(args, txn);
        if (cmd_upper == "SUNION") return SUnionCommand().execute(args, txn);
        if (cmd_upper == "SADD") return SAddCommand().execute(args, txn);
        if (cmd_upper == "SISMEMBER") return SIsMemberCommand().execute(args, txn);
        if (cmd_upper == "SMEMBERS") return SMembersCommand().execute(args, txn);
        if (cmd_upper == "SREM") return SRemCommand().execute(args, txn);
        if (cmd_upper == "LLEN") return LLenCommand().execute(args, txn);
        if (cmd_upper == "LPOP") return LPopCommand().execute(args, txn);
        if (cmd_upper == "LPUSH") return LPushCommand().execute(args, txn);
        if (cmd_upper == "LRANGE") return LRangeCommand().execute(args, txn);
        if (cmd_upper == "RPOP") return RPopCommand().execute(args, txn);
        if (cmd_upper == "RPUSH") return RPushCommand().execute(args, txn);
        if (cmd_upper == "HDEL") return HDelCommand().execute(args, txn);
        if (cmd_upper == "HEXISTS") return HExistsCommand().execute(args, txn);
        if (cmd_upper == "HGETALL") return HGetAllCommand().execute(args, txn);
        if (cmd_upper == "HGET") return HGetCommand().execute(args, txn);
        if (cmd_upper == "HSET") return HSetCommand().execute(args, txn);

        return "-ERR unknown command '" + cmd_upper + "'\r\n";
    };

    if (heavyCommands.count(cmd_upper)) {
        auto futureResult = globalThreadPool.enqueue(task);
        return futureResult.get();  
    } 
    
    else 
        return task();
}


std::string CommandDispatcher::handleTransactionCommand(const std::string& cmd, TransactionContext& txn) {
    if (cmd == "MULTI") return MultiCommand().execute({}, txn);
    if (cmd == "DISCARD") return DiscardCommand().execute({}, txn);
    if (cmd == "EXEC") return ExecCommand().execute({}, txn);

    return "-ERR unknown transaction command\r\n";
}

std::string CommandDispatcher::dispatch(const std::vector<std::string>& tokens, TransactionContext& txn) {
    if (tokens.empty()) return "-ERR empty command\r\n";

    std::string cmd = tokens[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
    std::vector<std::string> args(tokens.begin() + 1, tokens.end());

    if (isTransactionCommand(cmd)) {
        return handleTransactionCommand(cmd, txn);
    }

    if (txn.in_transaction) {
        txn.queued_commands.push_back(tokens);
        return "+QUEUED\r\n";
    }

    return executeCommand(cmd, args, txn);
}