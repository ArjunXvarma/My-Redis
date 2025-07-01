#include "datastore/datastore.hpp"

void DataStore::lpush(const std::string key, const std::vector<std::string>& values) {
    size_t idx = getShardIndex(key);
    std::unique_lock<std::shared_mutex> l(shards[idx].mutex);

    auto& shard_map = shards[idx].map;
    if (shard_map.find(key) == shard_map.end())
        shard_map[key] = {ValueType::LIST, std::deque<std::string>()};

    if (shard_map[key].type != ValueType::LIST)
        throw std::runtime_error("Key exists but is not of type LIST");

    auto& list = std::get<std::deque<std::string>>(shard_map[key].data);
    for (const auto& value : values) list.push_front(value);
}

void DataStore::rpush(const std::string key, const std::vector<std::string> &values) {
    size_t idx = getShardIndex(key);
    std::unique_lock<std::shared_mutex> l(shards[idx].mutex);

    auto& shard_map = shards[idx].map;
    if (shard_map.find(key) == shard_map.end())
        shard_map[key] = {ValueType::LIST, std::deque<std::string>()};

    if (shard_map[key].type != ValueType::LIST)
        throw std::runtime_error("Key exists but is not of type LIST");

    auto& list = std::get<std::deque<std::string>>(shard_map[key].data);
    for (const auto& value : values) list.push_back(value);
}

void DataStore::rpop(const std::string key, const int n) {
    size_t idx = getShardIndex(key);
    std::unique_lock<std::shared_mutex> l(shards[idx].mutex);

    auto& shard_map = shards[idx].map;
    auto it = shard_map.find(key);
    if (it == shard_map.end())
        throw std::runtime_error("Key does not exist");

    if (it->second.type != ValueType::LIST)
        throw std::runtime_error("Key exists but is not of type LIST");

    auto& list = std::get<std::deque<std::string>>(it->second.data);
    for (int i = 0; i < n && !list.empty(); ++i)
        list.pop_back();

    if (list.empty()) shard_map.erase(key);
}

void DataStore::lpop(const std::string key, const int n) {
    size_t idx = getShardIndex(key);
    std::unique_lock<std::shared_mutex> l(shards[idx].mutex);

    auto& shard_map = shards[idx].map;
    auto it = shard_map.find(key);
    if (it == shard_map.end())
        throw std::runtime_error("Key does not exist");

    if (it->second.type != ValueType::LIST)
        throw std::runtime_error("Key exists but is not of type LIST");

    auto& list = std::get<std::deque<std::string>>(it->second.data);
    for (int i = 0; i < n && !list.empty(); ++i)
        list.pop_front();

    if (list.empty()) shard_map.erase(key);
}

std::vector<std::string> DataStore::lrange(const std::string& key, int start, int end) {
    size_t idx = getShardIndex(key);
    std::shared_lock<std::shared_mutex> l(shards[idx].mutex);

    auto& shard_map = shards[idx].map;
    auto it = shard_map.find(key);
    if (it == shard_map.end()) throw std::runtime_error("Key does not exist");

    if (it->second.type != ValueType::LIST) throw std::runtime_error("Key exists but is not of type LIST");

    auto& list = std::get<std::deque<std::string>>(it->second.data);

    int size = static_cast<int>(list.size());

    // Normalize start and end indices
    if (start < 0) start += size;
    if (end < 0) end += size;

    // Clamp start and end to valid bounds
    start = std::max(0, std::min(start, size));
    end = std::max(0, std::min(end, size - 1));

    // If start > end, return an empty result
    if (start > end) return {};

    // Extract the range of elements
    std::vector<std::string> result;
    auto itStart = list.begin() + start;
    auto itEnd = list.begin() + end + 1;
    result.assign(itStart, itEnd);

    return result;
}

size_t DataStore::llen(const std::string& key) {
    size_t idx = getShardIndex(key);
    std::shared_lock<std::shared_mutex> l(shards[idx].mutex);

    auto& shard_map = shards[idx].map;
    auto it = shard_map.find(key);
    if (it == shard_map.end())
        throw std::runtime_error("Key does not exist");

    if (it->second.type != ValueType::LIST)
        throw std::runtime_error("Key exists but is not of type LIST");

    auto& list = std::get<std::deque<std::string>>(it->second.data);
    return list.size();
}