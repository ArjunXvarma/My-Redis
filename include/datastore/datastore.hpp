#pragma once

#include <unordered_map>
#include <string>
#include <optional>

using namespace std;

class DataStore {
public:
    static DataStore& getInstance();
    void set(const string& key, const string& value);
    optional<string> get(const string& key) const;

private:
    unordered_map<string, string> store;

    DataStore() = default;
    ~DataStore() = default;
    DataStore(const DataStore&) = delete;
    DataStore& operator=(const DataStore&) = delete;
};
