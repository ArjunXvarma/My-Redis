#include "protocol/RESPparser.hpp"

#include "protocol/RESPparser.hpp"
#include <sstream> // <-- Make sure this is included!

std::vector<std::string> RESPParser::parse(const std::string& input) {
    std::vector<std::string> result;
    size_t i = 0;

    if (input.empty() || input[i] != '*') return result;
    ++i;

    // Parse array length
    size_t arrayLenEnd = input.find("\r\n", i);
    if (arrayLenEnd == std::string::npos) return result;
    int count = std::stoi(input.substr(i, arrayLenEnd - i));
    i = arrayLenEnd + 2;

    for (int k = 0; k < count; ++k) {
        if (i >= input.size() || input[i] != '$') return {}; // incomplete
        ++i;

        size_t lenEnd = input.find("\r\n", i);
        if (lenEnd == std::string::npos) return {};
        int len = std::stoi(input.substr(i, lenEnd - i));
        i = lenEnd + 2;

        if (i + len > input.size()) return {}; // incomplete argument
        std::string arg = input.substr(i, len);
        result.push_back(arg);
        i += len;

        // Now skip the trailing \r\n after the argument
        if (i + 2 > input.size() || input.substr(i, 2) != "\r\n") return {};
        i += 2;
    }

    return result;
}

std::vector<std::string> RESPParser::split(const std::string& input) {
    std::istringstream iss(input);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}