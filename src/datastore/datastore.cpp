#include "datastore/datastore.hpp"

DataStore::DataStore() {
    extern std::string dumpFileName;
    store = PersistenceManager::load();
}

DataStore& DataStore::getInstance() {
    static DataStore instance;
    return instance;
}

void DataStore::set(const std::string& key, const std::string& value) {
    std::unique_lock<std::mutex> l(m);
    store[key] = Value{ValueType::STRING, value};
    save();
}

std::optional<std::string> DataStore::get(const std::string& key) const {
    std::unique_lock<std::mutex> l(m);
    auto it = store.find(key);
    if (it != store.end() && it->second.type == ValueType::STRING) {
        return std::get<std::string>(it->second.data);
    }
    return std::nullopt;
}

bool DataStore::del(const std::string& key) {
    std::unique_lock<std::mutex> l(m);
    return store.erase(key) > 0;
}


void DataStore::save() {
    PersistenceManager::save(this->store);
}

void DataStore::flushAll() {
    std::unique_lock<std::mutex> l(m);
    store.clear();
}

std::vector<std::string> DataStore::allKeys() {
    std::unique_lock<std::mutex> lock(m);
    std::vector<std::string> keys;
    keys.reserve(store.size());

    for (const auto& [key, _] : store) 
        keys.push_back(key);

    return keys;
}

std::vector<std::optional<std::string>> DataStore::mget(const std::vector<std::string>& keys) {
    std::unique_lock<std::mutex> l(m);
    std::vector<std::optional<std::string>> res;
    res.reserve(keys.size());

    for (const auto& key : keys) {
        auto it = store.find(key);
        if (it != store.end() && it->second.type == ValueType::STRING) 
            res.push_back(std::get<std::string>(it->second.data));
        
        else res.push_back(std::nullopt); 
    }

    return res;
}

void DataStore::mset(const std::unordered_map<std::string, std::string>& kvs) {
    std::unique_lock<std::mutex> l(m);
    for (const auto& [key, value] : kvs) 
        store[key] = Value{ValueType::STRING, value};
}