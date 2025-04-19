#include "datastore/datastore.hpp"

DataStore& DataStore::getInstance() {
    static DataStore instance;
    return instance;
}

void DataStore::set(const string& key, const string& value) {
    store[key] = value;
}

optional<string> DataStore::get(const string& key) const {
    auto it = store.find(key);
    if (it != store.end()) return it->second;
    return nullopt;
}