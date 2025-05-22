#pragma once

#include <variant>
#include <string>
#include <deque>
#include <unordered_set>
#include <unordered_map>

enum class ValueType { STRING, LIST, SET, HASH };

struct Value {
    ValueType type;
    std::variant<std::string, std::deque<std::string>, 
                 std::unordered_set<std::string>, 
                 std::unordered_map<std::string, std::string>> data;
};