// #include "datastore/datastore.hpp"

// int DataStore::sadd(const std::string key, const std::vector<std::string> &values) {
    // std::unique_lock<std::shared_mutex> l(m);

    // if (store.find(key) == store.end()) 
    //     store[key] = {ValueType::SET, std::unordered_set<std::string>()};
    
    // if (store[key].type != ValueType::SET) 
    //     throw std::runtime_error("Key exists but is not of type LIST");
    
    // auto& set = std::get<std::unordered_set<std::string>>(store[key].data);
    // int addedCount = 0;

    // for (const auto& value : values) {
    //     if (set.insert(value).second) 
    //         ++addedCount;
    // }

    // return addedCount;
// }

// int DataStore::srem(const std::string key, const std::vector<std::string> &values) {
    // std::unique_lock<std::shared_mutex> l(m);

    // if (store.find(key) == store.end()) 
    //     throw std::runtime_error("Key deos not exists");

    // if (store[key].type != ValueType::SET) 
    //     throw std::runtime_error("Key exists but is not of type LIST");
    
    // auto& set = std::get<std::unordered_set<std::string>>(store[key].data);
    // int removedCount = 0;

    // for (const auto& value : values) {
    //     if (set.erase(value)) 
    //         ++removedCount;
    // }

    // return removedCount;
// }

// std::vector<std::string> DataStore::smembers(const std::string key) {
//     std::unique_lock<std::shared_mutex> l(m);

//     std::vector<std::string> res;
//     if (store.find(key) == store.end()) 
//         throw std::runtime_error("Key deos not exists");
    
//     if (store[key].type != ValueType::SET) 
//         throw std::runtime_error("Key exists but is not of type LIST");
    
//     auto& list = std::get<std::unordered_set<std::string>>(store[key].data);
    
//     for (const auto& value : list) res.push_back(value);

//     return res;
// }

// bool DataStore::sismember(const std::string key, const std::string value) {
//     std::unique_lock<std::shared_mutex> l(m);

//     if (store.find(key) == store.end()) 
//         throw std::runtime_error("Key deos not exists");
    
//     if (store[key].type != ValueType::SET) 
//         throw std::runtime_error("Key exists but is not of type LIST");
    
//     auto& list = std::get<std::unordered_set<std::string>>(store[key].data);
    
//     return list.find(value) != list.end();
// }

#include "datastore/datastore.hpp"

int DataStore::sadd(const std::string key, const std::vector<std::string>& values) {
    size_t idx = getShardIndex(key);
    std::unique_lock<std::shared_mutex> l(shards[idx].mutex);

    auto& shard_map = shards[idx].map;
    if (shard_map.find(key) == shard_map.end())
        shard_map[key] = {ValueType::SET, std::unordered_set<std::string>()};

    if (shard_map[key].type != ValueType::SET)
        throw std::runtime_error("Key exists but is not of type SET");

    auto& set = std::get<std::unordered_set<std::string>>(shard_map[key].data);

    int addedCount = 0;
    for (const auto& value : values) {
        if (set.insert(value).second) 
            ++addedCount;
    }

    return addedCount;
}

int DataStore::srem(const std::string key, const std::vector<std::string>& values) {
    size_t idx = getShardIndex(key);
    std::unique_lock<std::shared_mutex> l(shards[idx].mutex);

    auto& shard_map = shards[idx].map;
    auto it = shard_map.find(key);
    if (it == shard_map.end())
        throw std::runtime_error("Key does not exist");

    if (it->second.type != ValueType::SET)
        throw std::runtime_error("Key exists but is not of type SET");

    auto& set = std::get<std::unordered_set<std::string>>(it->second.data);
    int removedCount = 0;

    for (const auto& value : values) {
        if (set.erase(value)) 
            ++removedCount;
    }

    return removedCount;
}

bool DataStore::sismember(const std::string key, const std::string value) {
    size_t idx = getShardIndex(key);
    std::shared_lock<std::shared_mutex> l(shards[idx].mutex);

    auto& shard_map = shards[idx].map;
    auto it = shard_map.find(key);
    if (it == shard_map.end())
        return false;

    if (it->second.type != ValueType::SET)
        throw std::runtime_error("Key exists but is not of type SET");

    auto& set = std::get<std::unordered_set<std::string>>(it->second.data);
    return set.find(value) != set.end();
}

std::vector<std::string> DataStore::smembers(const std::string key) {
    size_t idx = getShardIndex(key);
    std::shared_lock<std::shared_mutex> l(shards[idx].mutex);

    auto& shard_map = shards[idx].map;
    auto it = shard_map.find(key);
    if (it == shard_map.end())
        throw std::runtime_error("Key does not exist");

    if (it->second.type != ValueType::SET)
        throw std::runtime_error("Key exists but is not of type SET");

    auto& set = std::get<std::unordered_set<std::string>>(it->second.data);
    return std::vector<std::string>(set.begin(), set.end());
}