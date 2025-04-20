#include "protocol/RESPencoder.hpp"

string RESPEncoder::encodeSimpleString(const string& value) {
    return "+" + value + "\r\n";
}

string RESPEncoder::encodeBulkString(const string& value) {
    return "$" + to_string(value.size()) + "\r\n" + value + "\r\n";
}

string RESPEncoder::encodeError(const string& error) {
    return "-" + error + "\r\n";
}
