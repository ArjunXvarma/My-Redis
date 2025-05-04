#include "protocol/RESPencoder.hpp"

std::string RESPEncoder::encodeSimpleString(const std::string& value) {
    return "+" + value + "\r\n";
}

std::string RESPEncoder::encodeBulkString(const std::string& value) {
    return "$" + std::to_string(value.size()) + "\r\n" + value + "\r\n";
}

std::string RESPEncoder::encodeError(const std::string& error) {
    return "-" + error + "\r\n";
}
