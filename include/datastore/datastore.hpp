#pragma once

#include <unordered_map>
#include <string>
#include <optional>
#include <iostream>

#include "../persistence/persistenceManager.hpp"

using namespace std;

class DataStore {
public:
    static DataStore& getInstance();
    void set(const string& key, const string& value);
    void del(const string& key);
    optional<string> get(const string& key) const;
    void save();

private:
    unordered_map<string, string> store;

    DataStore();
    DataStore(const DataStore&) = delete;
    DataStore& operator=(const DataStore&) = delete;
};
