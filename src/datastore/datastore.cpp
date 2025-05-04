#include "datastore/datastore.hpp"

DataStore::DataStore() {
    extern string dumpFileName;
    store = PersistenceManager::load();
}

DataStore& DataStore::getInstance() {
    static DataStore instance;
    return instance;
}

void DataStore::set(const std::string& key, const std::string& value) {
    store[key] = value;
    save();
}

void DataStore::del(const std::string& key) {
    store.erase(key);
    save();
}

optional<string> DataStore::get(const std::string& key) const {
    auto it = store.find(key);
    if (it != store.end()) return it->second;
    return nullopt;
}

void DataStore::save() {
    PersistenceManager::save(this->store);
}