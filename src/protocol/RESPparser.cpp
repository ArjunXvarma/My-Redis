#include "protocol/RESPparser.hpp"

std::vector<std::string> RESPParser::parse(const std::string& input) {
    std::vector<std::string> result;
    size_t i = 0;

    if (input[i] != '*') return result;  
    ++i;

    // Parse array length
    size_t arrayLenEnd = input.find("\r\n", i);
    int count = std::stoi(input.substr(i, arrayLenEnd - i));
    i = arrayLenEnd + 2;

    for (int k = 0; k < count; ++k) {
        if (input[i] != '$') break;
        ++i;

        size_t lenEnd = input.find("\r\n", i);
        int len = stoi(input.substr(i, lenEnd - i));
        i = lenEnd + 2;

        std::string arg = input.substr(i, len);
        result.push_back(arg);
        i += len + 2; 
    }

    return result;
}
