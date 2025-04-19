#pragma once

#include <unordered_map>
#include <string>
#include <optional>

using namespace std;

class DataStore {
public:
    static DataStore& getInstance() {
        static DataStore instance;
        return instance;
    }

    void set(const std::string& key, const string& value) {
        store[key] = value;
    }

    optional<string> get(const string& key) const {
        auto it = store.find(key);
        if (it != store.end()) return it->second;
        return nullopt;
    }

private:
    unordered_map<string, string> store;

    DataStore() = default;
    ~DataStore() = default;
    DataStore(const DataStore&) = delete;
    DataStore& operator=(const DataStore&) = delete;
};
