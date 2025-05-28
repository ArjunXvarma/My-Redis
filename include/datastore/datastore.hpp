#pragma once

#include <unordered_map>
#include <string>
#include <optional>
#include <iostream>
#include <mutex>
#include <vector>
#include <algorithm>

#include "../persistence/persistenceManager.hpp"
#include "Value.hpp"

class DataStore {
private:
    std::unordered_map<std::string, Value> store;
    mutable std::mutex m;
    
public:
    DataStore();
    DataStore(const DataStore&) = delete;

    static DataStore& getInstance();

    // List commands
    void lpush(const std::string key, const std::vector<std::string> &values);
    void rpush(const std::string key, const std::vector<std::string> &values);
    void lpop(const std::string key, const int n);
    void rpop(const std::string key, const int n);
    std::vector<std::string> lrange(const std::string& key, int start, int end);
    size_t llen(const std::string& key);

    // Set commands
    int sadd(const std::string key, const std::vector<std::string> &values);
    int srem(const std::string key, const std::vector<std::string> &values);
    std::vector<std::string> smembers(const std::string key);
    bool sismember(const std::string key, const std::string value);

    // Hash Set commands
    void hset(const std::string hash_key, const std::string key, const std::string value);
    std::string hget(const std::string hash_key, const std::string key);
    void hdel(const std::string hash_key, const std::string key);
    std::vector<std::string> hgetall(const std::string hash_key);
    bool hexists(const std::string hash_key, const std::string key);

    void set(const std::string& key, const std::string &value);
    bool del(const std::string& key);
    std::optional<std::string> get(const std::string &key) const;
    void flushAll();
    std::vector<std::string> allKeys();
    std::vector<std::optional<std::string>> mget(const std::vector<std::string> &keys);
    void mset(const std::unordered_map<std::string, std::string>& kvs);
    std::vector<std::string> scan();
    std::vector<std::string> sinter(std::vector<std::string> keys);
    std::vector<std::string> sort(const std::string& key, const std::string& order = "ASC", int offset = 0, int count = -1);
    std::vector<std::string> sunion(std::vector<std::string> keys);
    void zrange();
    
    void save();
    
    DataStore& operator=(const DataStore&) = delete;
};
