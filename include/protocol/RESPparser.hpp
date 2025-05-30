#pragma once
#include <string>
#include <vector>
#include <sstream>

class RESPParser {
public:
    static std::vector<std::string> parse(const std::string& input);
    static std::vector<std::string> split(const std::string& input);
};