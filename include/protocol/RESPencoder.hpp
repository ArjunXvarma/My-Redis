#pragma once
#include <string>

class RESPEncoder {
public:
    static std::string encodeSimpleString(const std::string& value);
    static std::string encodeBulkString(const std::string& value);
    static std::string encodeError(const std::string& error);
};
