#include "datastore/datastore.hpp"
#include <shared_mutex>
#include <set>
#include <algorithm>

std::array<Shard, NUM_SHARDS> shards;

size_t getShardIndex(const std::string& key) {
    if (key.empty()) throw std::invalid_argument("Key is empty: cannot compute shard index.");
    
    return std::hash<std::string>{}(key) % NUM_SHARDS;
}

bool shardExists(const std::string& key) {
    size_t idx = getShardIndex(key);
    std::shared_lock lock(shards[idx].mutex);
    return shards[idx].map.find(key) != shards[idx].map.end();
}

DataStore::DataStore() {
   
}

DataStore& DataStore::getInstance() {
    static DataStore instance;
    return instance;
}

void DataStore::set(const std::string& key, const std::string& value) {
    size_t idx = getShardIndex(key);
    std::unique_lock lock(shards[idx].mutex);
    shards[idx].map[key] = Value{ValueType::STRING, value};
}

std::optional<std::string> DataStore::get(const std::string& key) const {
    size_t idx = getShardIndex(key);
    std::shared_lock lock(shards[idx].mutex);
    auto it = shards[idx].map.find(key);
    if (it != shards[idx].map.end() && it->second.type == ValueType::STRING)
        return std::get<std::string>(it->second.data);
    return std::nullopt;
}

bool DataStore::del(const std::string& key) {
    size_t idx = getShardIndex(key);
    std::unique_lock lock(shards[idx].mutex);
    return shards[idx].map.erase(key) > 0;
}

void DataStore::save() {
    // If you want to persist, gather all shards' data
    std::unordered_map<std::string, Value> merged;
    for (size_t i = 0; i < NUM_SHARDS; ++i) {
        std::shared_lock lock(shards[i].mutex);
        merged.insert(shards[i].map.begin(), shards[i].map.end());
    }
    PersistenceManager::save(merged);
}

void DataStore::flushAll() {
    std::array<std::unique_lock<std::shared_mutex>, NUM_SHARDS> locks;
    for (size_t i = 0; i < NUM_SHARDS; ++i)
        locks[i] = std::unique_lock<std::shared_mutex>(shards[i].mutex);
    for (auto& shard : shards)
        shard.map.clear();
}

std::vector<std::string> DataStore::allKeys() {
    std::array<std::shared_lock<std::shared_mutex>, NUM_SHARDS> locks;
    for (size_t i = 0; i < NUM_SHARDS; ++i)
        locks[i] = std::shared_lock<std::shared_mutex>(shards[i].mutex);

    std::vector<std::string> keys;
    size_t total_keys = 0;
    for (const auto& shard : shards)
        total_keys += shard.map.size();
    keys.reserve(total_keys);

    for (const auto& shard : shards) {
        for (const auto& [key, _] : shard.map)
            keys.push_back(key);
    }
    return keys;
}

std::vector<std::optional<std::string>> DataStore::mget(const std::vector<std::string>& keys) {
    // Lock all relevant shards for reading (avoid duplicate locks)
    std::set<size_t> shard_indices;
    for (const auto& key : keys)
        shard_indices.insert(getShardIndex(key));
    std::map<size_t, std::shared_lock<std::shared_mutex>> locks;
    for (auto idx : shard_indices)
        locks.emplace(idx, std::shared_lock<std::shared_mutex>(shards[idx].mutex));

    std::vector<std::optional<std::string>> res;
    res.reserve(keys.size());
    for (const auto& key : keys) {
        size_t idx = getShardIndex(key);
        auto it = shards[idx].map.find(key);
        if (it != shards[idx].map.end() && it->second.type == ValueType::STRING)
            res.push_back(std::get<std::string>(it->second.data));
        else
            res.push_back(std::nullopt);
    }
    return res;
}

void DataStore::mset(const std::unordered_map<std::string, std::string>& kvs) {
    // Lock all relevant shards for writing (avoid duplicate locks)
    std::set<size_t> shard_indices;
    for (const auto& [key, _] : kvs)
        shard_indices.insert(getShardIndex(key));
    std::map<size_t, std::unique_lock<std::shared_mutex>> locks;
    for (auto idx : shard_indices)
        locks.emplace(idx, std::unique_lock<std::shared_mutex>(shards[idx].mutex));

    for (const auto& [key, value] : kvs) {
        size_t idx = getShardIndex(key);
        shards[idx].map[key] = Value{ValueType::STRING, value};
    }
}

std::vector<std::string> DataStore::sort(const std::string& key, const std::string& order, int offset, int count) {
    size_t idx = getShardIndex(key);
    std::shared_lock lock(shards[idx].mutex);
    auto it = shards[idx].map.find(key);
    if (it == shards[idx].map.end())
        return {};

    std::vector<std::string> elements;
    if (it->second.type == ValueType::LIST) {
        const auto& list = std::get<std::deque<std::string>>(it->second.data);
        elements.assign(list.begin(), list.end());
    } else if (it->second.type == ValueType::SET) {
        const auto& set = std::get<std::unordered_set<std::string>>(it->second.data);
        elements.assign(set.begin(), set.end());
    } else return {};

    if (order == "DESC")
        std::sort(elements.begin(), elements.end(), std::greater<>());
    else
        std::sort(elements.begin(), elements.end());

    int total = static_cast<int>(elements.size());
    int start = std::max(0, offset);
    int end = (count < 0) ? total : std::min(start + count, total);

    if (start > total) return {};

    return std::vector<std::string>(elements.begin() + start, elements.begin() + end);
}

std::vector<std::string> DataStore::sinter(std::vector<std::string> keys) {
    if (keys.empty()) return {};

    // Lock all relevant shards for reading
    std::set<size_t> shard_indices;
    for (const auto& key : keys)
        shard_indices.insert(getShardIndex(key));
    std::map<size_t, std::shared_lock<std::shared_mutex>> locks;
    for (auto idx : shard_indices)
        locks.emplace(idx, std::shared_lock<std::shared_mutex>(shards[idx].mutex));

    // Find the first valid set
    size_t idx0 = getShardIndex(keys[0]);
    auto it = shards[idx0].map.find(keys[0]);
    if (it == shards[idx0].map.end() || it->second.type != ValueType::SET)
        return {};

    std::unordered_set<std::string> result = std::get<std::unordered_set<std::string>>(it->second.data);

    for (size_t i = 1; i < keys.size(); ++i) {
        size_t idx = getShardIndex(keys[i]);
        auto it2 = shards[idx].map.find(keys[i]);
        if (it2 == shards[idx].map.end() || it2->second.type != ValueType::SET)
            return {};
        const auto& nextSet = std::get<std::unordered_set<std::string>>(it2->second.data);
        for (auto rit = result.begin(); rit != result.end(); ) {
            if (nextSet.find(*rit) == nextSet.end())
                rit = result.erase(rit);
            else
                ++rit;
        }
    }
    return std::vector<std::string>(result.begin(), result.end());
}

std::vector<std::string> DataStore::sunion(std::vector<std::string> keys) {
    // Lock all relevant shards for reading
    std::set<size_t> shard_indices;
    for (const auto& key : keys)
        shard_indices.insert(getShardIndex(key));
    
    std::map<size_t, std::shared_lock<std::shared_mutex>> locks;
    for (auto idx : shard_indices)
        locks.emplace(idx, std::shared_lock<std::shared_mutex>(shards[idx].mutex));

    std::unordered_set<std::string> res;
    for (const std::string &key : keys) {
        size_t idx = getShardIndex(key);
        auto it = shards[idx].map.find(key);
        if (it == shards[idx].map.end() || it->second.type != ValueType::SET)
            continue;
        const auto& set = std::get<std::unordered_set<std::string>>(it->second.data);
        res.insert(set.begin(), set.end());
    }
    return std::vector<std::string>(res.begin(), res.end());
}