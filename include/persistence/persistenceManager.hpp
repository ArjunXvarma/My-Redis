#pragma once
#include <unordered_map>
#include <string>

#include "../utils/GlobalThreadPool.hpp"

class PersistenceManager {
public:
    static void save(const std::unordered_map<std::string, std::string>& store);
    static std::unordered_map<std::string, std::string> load();
};