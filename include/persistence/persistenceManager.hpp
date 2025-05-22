#pragma once
#include <unordered_map>
#include <string>
#include <optional>

#include "../utils/GlobalThreadPool.hpp"
#include "../datastore/Value.hpp"

class PersistenceManager {
public:
    static std::unordered_map<std::string, Value> load();
    static void save(const std::unordered_map<std::string, Value>& data);
};