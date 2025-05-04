#pragma once

#include <unordered_map>
#include <string>
#include <optional>
#include <iostream>

#include "../persistence/persistenceManager.hpp"

class DataStore {
public:
    static DataStore& getInstance();
    void set(const std::string& key, const std::string& value);
    void del(const std::string& key);
    std::optional<std::string> get(const std::string& key) const;
    void save();

private:
    std::unordered_map<std::string, std::string> store;

    DataStore();
    DataStore(const DataStore&) = delete;
    DataStore& operator=(const DataStore&) = delete;
};
