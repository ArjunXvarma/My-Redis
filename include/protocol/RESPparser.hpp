#pragma once
#include <string>
#include <vector>

class RESPParser {
public:
    static std::vector<std::string> parse(const std::string& input);
};