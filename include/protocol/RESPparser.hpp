#pragma once
#include <string>
#include <vector>

using namespace std;

class RESPParser {
public:
    static vector<string> parse(const string& input);
};