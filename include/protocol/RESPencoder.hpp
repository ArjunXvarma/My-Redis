#pragma once
#include <string>

using namespace std;

class RESPEncoder {
public:
    static string encodeSimpleString(const string& value);
    static string encodeBulkString(const string& value);
    static string encodeError(const string& error);
};
