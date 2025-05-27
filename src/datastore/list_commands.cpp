#include "datastore/datastore.hpp"

void DataStore::lpush(const std::string key, const std::vector<std::string>& values) {
    std::unique_lock<std::mutex> l(m);

    if (store.find(key) == store.end()) 
        store[key] = {ValueType::LIST, std::deque<std::string>()};
    
    if (store[key].type != ValueType::LIST) 
        throw std::runtime_error("Key exists but is not of type LIST");
    
    auto& list = std::get<std::deque<std::string>>(store[key].data);
    
    for (const auto& value : values) list.push_front(value);
}

void DataStore::rpush(const std::string key, const std::vector<std::string> &values) {
    std::unique_lock<std::mutex> l(m);

    if (store.find(key) == store.end()) 
        store[key] = {ValueType::LIST, std::deque<std::string>()};
    
    if (store[key].type != ValueType::LIST) 
        throw std::runtime_error("Key exists but is not of type LIST");
    
    auto& list = std::get<std::deque<std::string>>(store[key].data);
    for (const auto& value : values) list.push_back(value);
}

void DataStore::rpop(const std::string key, const int n) {
    std::unique_lock<std::mutex> l(m);

    if (store.find(key) == store.end()) 
        throw std::runtime_error("Key does not exist");
    
    if (store[key].type != ValueType::LIST) 
        throw std::runtime_error("Key exists but is not of type LIST");
    
    auto& list = std::get<std::deque<std::string>>(store[key].data);
    for (int i = 0; i < n && !list.empty(); ++i) 
        list.pop_back();
    
    if (list.empty()) store.erase(key);
}

void DataStore::lpop(const std::string key, const int n) {
    std::unique_lock<std::mutex> l(m);

    if (store.find(key) == store.end()) 
        throw std::runtime_error("Key does not exist");

    if (store[key].type != ValueType::LIST) 
        throw std::runtime_error("Key exists but is not of type LIST");
    
    auto& list = std::get<std::deque<std::string>>(store[key].data);
    for (int i = 0; i < n && !list.empty(); ++i) 
        list.pop_front();
    
    if (list.empty()) store.erase(key);   
}

std::vector<std::string> DataStore::lrange(const std::string& key, int start, int end) {
    std::unique_lock<std::mutex> l(m);

    if (store.find(key) == store.end()) throw std::runtime_error("Key does not exist");
    
    if (store[key].type != ValueType::LIST) throw std::runtime_error("Key exists but is not of type LIST");

    auto& list = std::get<std::deque<std::string>>(store[key].data);

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
    std::unique_lock<std::mutex> l(m);

    if (store.find(key) == store.end()) 
        throw std::runtime_error("Key does not exist");
    
    if (store[key].type != ValueType::LIST) 
        throw std::runtime_error("Key exists but is not of type LIST");
    
    auto& list = std::get<std::deque<std::string>>(store[key].data);
    return list.size();
}