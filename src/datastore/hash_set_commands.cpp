#include "datastore/datastore.hpp"

void DataStore::hset(const std::string hash_key, const std::string key, const std::string value) {
    std::unique_lock<std::mutex> l(m);

    if (store.find(hash_key) == store.end()) 
        store[hash_key] = {ValueType::HASH, std::unordered_map<std::string, std::string>()};
    
    if (store[hash_key].type != ValueType::HASH) 
        throw std::runtime_error("Hash variable exists but is not of type LIST");
    
    auto& hash_set = std::get<std::unordered_map<std::string, std::string>>(store[hash_key].data);
    
    hash_set[key] = value;
}

std::string DataStore::hget(const std::string hash_key, const std::string key) {
    std::unique_lock<std::mutex> l(m);

    if (store.find(hash_key) == store.end()) 
        throw std::runtime_error("Hash key does not exist");

    if (store[hash_key].type != ValueType::HASH) 
        throw std::runtime_error("Hash variable exists but is not of type HASH");

    auto& hash_set = std::get<std::unordered_map<std::string, std::string>>(store[hash_key].data);
    if (hash_set.find(key) == hash_set.end()) 
        throw std::runtime_error("Key does not exist in the hash");

    return hash_set[key];
}

void DataStore::hdel(const std::string hash_key, const std::string key) {
    std::unique_lock<std::mutex> l(m);

    if (store.find(hash_key) == store.end()) 
        throw std::runtime_error("Hash key does not exist");

    if (store[hash_key].type != ValueType::HASH) 
        throw std::runtime_error("Hash variable exists but is not of type HASH");

    auto& hash_set = std::get<std::unordered_map<std::string, std::string>>(store[hash_key].data);
    hash_set.erase(key);

    if (hash_set.empty()) 
        store.erase(hash_key);
}

std::vector<std::string> DataStore::hgetall(const std::string hash_key) {
    std::unique_lock<std::mutex> l(m);

    if (store.find(hash_key) == store.end()) 
        throw std::runtime_error("Hash key does not exist");

    if (store[hash_key].type != ValueType::HASH) 
        throw std::runtime_error("Hash variable exists but is not of type HASH");

    auto& hash_set = std::get<std::unordered_map<std::string, std::string>>(store[hash_key].data);
    std::vector<std::string> result;
    for (const auto& [key, value] : hash_set) 
        result.push_back(key + ":" + value);

    return result;
}

bool DataStore::hexists(const std::string hash_key, const std::string key) {
    std::unique_lock<std::mutex> l(m);

    if (store.find(hash_key) == store.end()) 
        return false;

    if (store[hash_key].type != ValueType::HASH) 
        throw std::runtime_error("Hash variable exists but is not of type HASH");

    auto& hash_set = std::get<std::unordered_map<std::string, std::string>>(store[hash_key].data);
    return hash_set.find(key) != hash_set.end();
}
