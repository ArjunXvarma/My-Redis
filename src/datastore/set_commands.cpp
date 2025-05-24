#include "datastore/datastore.hpp"

int DataStore::sadd(const std::string key, const std::vector<std::string> &values) {
    std::unique_lock<std::mutex> l(m);

    if (store.find(key) == store.end()) 
        store[key] = {ValueType::SET, std::unordered_set<std::string>()};
    
    if (store[key].type != ValueType::SET) 
        throw std::runtime_error("Key exists but is not of type LIST");
    
    auto& set = std::get<std::unordered_set<std::string>>(store[key].data);
    int addedCount = 0;

    for (const auto& value : values) {
        if (set.insert(value).second) 
            ++addedCount;
    }

    return addedCount;
}

int DataStore::srem(const std::string key, const std::vector<std::string> &values) {
    std::unique_lock<std::mutex> l(m);

    if (store.find(key) == store.end()) 
        throw std::runtime_error("Key deos not exists");

    if (store[key].type != ValueType::SET) 
        throw std::runtime_error("Key exists but is not of type LIST");
    
    auto& set = std::get<std::unordered_set<std::string>>(store[key].data);
    int removedCount = 0;

    for (const auto& value : values) {
        if (set.erase(value)) 
            ++removedCount;
    }

    return removedCount;
}

std::vector<std::string> DataStore::smembers(const std::string key) {
    std::unique_lock<std::mutex> l(m);

    std::vector<std::string> res;
    if (store.find(key) == store.end()) 
        throw std::runtime_error("Key deos not exists");
    
    if (store[key].type != ValueType::SET) 
        throw std::runtime_error("Key exists but is not of type LIST");
    
    auto& list = std::get<std::unordered_set<std::string>>(store[key].data);
    
    for (const auto& value : list) res.push_back(value);

    return res;
}

bool DataStore::sismember(const std::string key, const std::string value) {
    std::unique_lock<std::mutex> l(m);

    if (store.find(key) == store.end()) 
        throw std::runtime_error("Key deos not exists");
    
    if (store[key].type != ValueType::SET) 
        throw std::runtime_error("Key exists but is not of type LIST");
    
    auto& list = std::get<std::unordered_set<std::string>>(store[key].data);
    
    return list.find(value) != list.end();
}