#include "datastore/datastore.hpp"

void DataStore::hset(const std::string hash_key, const std::string key, const std::string value) {
    size_t idx = getShardIndex(hash_key);
    std::unique_lock<std::shared_mutex> l(shards[idx].mutex);

    auto& shard_map = shards[idx].map;
    if (shard_map.find(hash_key) == shard_map.end())
        shard_map[hash_key] = {ValueType::HASH, std::unordered_map<std::string, std::string>()};

    if (shard_map[hash_key].type != ValueType::HASH)
        throw std::runtime_error("Hash variable exists but is not of type HASH");

    auto& hash_set = std::get<std::unordered_map<std::string, std::string>>(shard_map[hash_key].data);
    hash_set[key] = value;
}

std::string DataStore::hget(const std::string hash_key, const std::string key) {
    size_t idx = getShardIndex(hash_key);
    std::shared_lock<std::shared_mutex> l(shards[idx].mutex);

    auto& shard_map = shards[idx].map;
    auto it = shard_map.find(hash_key);
    if (it == shard_map.end())
        throw std::runtime_error("Hash key does not exist");

    if (it->second.type != ValueType::HASH)
        throw std::runtime_error("Hash variable exists but is not of type HASH");

    auto& hash_set = std::get<std::unordered_map<std::string, std::string>>(it->second.data);
    auto hit = hash_set.find(key);
    if (hit == hash_set.end())
        throw std::runtime_error("Key does not exist in the hash");

    return hit->second;
}

void DataStore::hdel(const std::string hash_key, const std::string key) {
    size_t idx = getShardIndex(hash_key);
    std::unique_lock<std::shared_mutex> l(shards[idx].mutex);

    auto& shard_map = shards[idx].map;
    auto it = shard_map.find(hash_key);
    if (it == shard_map.end())
        throw std::runtime_error("Hash key does not exist");

    if (it->second.type != ValueType::HASH)
        throw std::runtime_error("Hash variable exists but is not of type HASH");

    auto& hash_set = std::get<std::unordered_map<std::string, std::string>>(it->second.data);
    hash_set.erase(key);

    if (hash_set.empty())
        shard_map.erase(hash_key);
}

std::vector<std::string> DataStore::hgetall(const std::string hash_key) {
    size_t idx = getShardIndex(hash_key);
    std::shared_lock<std::shared_mutex> l(shards[idx].mutex);

    auto& shard_map = shards[idx].map;
    auto it = shard_map.find(hash_key);
    if (it == shard_map.end())
        throw std::runtime_error("Hash key does not exist");

    if (it->second.type != ValueType::HASH)
        throw std::runtime_error("Hash variable exists but is not of type HASH");

    auto& hash_set = std::get<std::unordered_map<std::string, std::string>>(it->second.data);
    std::vector<std::string> result;
    for (const auto& [key, value] : hash_set)
        result.push_back(key + ":" + value);

    return result;
}

bool DataStore::hexists(const std::string hash_key, const std::string key) {
    size_t idx = getShardIndex(hash_key);
    std::shared_lock<std::shared_mutex> l(shards[idx].mutex);

    auto& shard_map = shards[idx].map;
    auto it = shard_map.find(hash_key);
    if (it == shard_map.end())
        return false;

    if (it->second.type != ValueType::HASH)
        throw std::runtime_error("Hash variable exists but is not of type HASH");

    auto& hash_set = std::get<std::unordered_map<std::string, std::string>>(it->second.data);
    return hash_set.find(key) != hash_set.end();
}