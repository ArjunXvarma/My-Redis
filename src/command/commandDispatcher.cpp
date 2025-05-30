#include "command/commandDispatcher.hpp"
#include "utils/GlobalThreadPool.hpp"

#ifdef __linux__
ThreadPool globalThreadPool;
#endif

CommandDispatcher::CommandDispatcher() {}

std::string CommandDispatcher::dispatch(const std::vector<std::string>& tokens, TransactionContext& txn) {
    if (tokens.empty()) return "-ERR empty command\r\n";

    std::string cmd = tokens[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
    std::vector<std::string> args(tokens.begin() + 1, tokens.end());
    
    auto futureResult = globalThreadPool.enqueue([cmd, args]() -> std::string {
        if (cmd == "PING") return PingCommand().execute(args);
        if (cmd == "ECHO") return EchoCommand().execute(args);
        if (cmd == "SET") return SetCommand().execute(args);
        if (cmd == "GET") return GetCommand().execute(args);

        if (cmd == "DEL") return DelCommand().execute(args);
        if (cmd == "FLUSHALL") return FlushAllCommand().execute(args);
        if (cmd == "KEYS") return KeysCommand().execute(args);
        if (cmd == "MGET") return MGetCommand().execute(args);
        if (cmd == "MSET") return MSetCommand().execute(args);
        if (cmd == "SINTER") return SInterCommand().execute(args);
        if (cmd == "SORT") return SortCommand().execute(args);
        if (cmd == "SUNION") return SUnionCommand().execute(args);

        // Set commands
        if (cmd == "SADD") return SAddCommand().execute(args);
        if (cmd == "SISMEMBER") return SIsMemberCommand().execute(args);
        if (cmd == "SMEMBERS") return SMembersCommand().execute(args);
        if (cmd == "SREM") return SRemCommand().execute(args);

        // List Commands
        if (cmd == "LLEN") return LLenCommand().execute(args);
        if (cmd == "LPOP") return LPopCommand().execute(args);
        if (cmd == "LPUSH") return LPushCommand().execute(args);
        if (cmd == "LRANGE") return LRangeCommand().execute(args);
        if (cmd == "RPOP") return RPopCommand().execute(args);
        if (cmd == "RPUSH") return RPushCommand().execute(args);

        // Hash set commands
        if (cmd == "HDEL") return HDelCommand().execute(args);
        if (cmd == "HEXISTS") return HExistsCommand().execute(args);
        if (cmd == "HGETALL") return HGetAllCommand().execute(args);
        if (cmd == "HGET") return HGetCommand().execute(args);
        if (cmd == "HSET") return HSetCommand().execute(args);

        return "-ERR unknown command '" + cmd + "'\r\n";
    });

    return futureResult.get();
}