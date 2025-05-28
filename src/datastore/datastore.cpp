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

std::vector<std::string> DataStore::sort(const std::string& key, const std::string& order, int offset, int count) {
    std::unique_lock<std::mutex> l(m);

    auto it = store.find(key);
    if (it == store.end())
        throw std::runtime_error("Key does not exist");

    std::vector<std::string> elements;

    // Support sorting for LIST and SET types
    if (it->second.type == ValueType::LIST) {
        const auto& list = std::get<std::deque<std::string>>(it->second.data);
        elements.assign(list.begin(), list.end());
    } 
    
    else if (it->second.type == ValueType::SET) {
        const auto& set = std::get<std::unordered_set<std::string>>(it->second.data);
        elements.assign(set.begin(), set.end());
    } 
    
    else 
        throw std::runtime_error("SORT only supported for LIST or SET types");
    

    // Sort elements
    if (order == "DESC")
        std::sort(elements.begin(), elements.end(), std::greater<>());
    else
        std::sort(elements.begin(), elements.end());

    // Apply offset and count
    int total = static_cast<int>(elements.size());
    int start = std::max(0, offset);
    int end = (count < 0) ? total : std::min(start + count, total);

    if (start > total) return {};

    return std::vector<std::string>(elements.begin() + start, elements.begin() + end);
}

std::vector<std::string> DataStore::sinter(std::vector<std::string> keys) {
    std::unique_lock<std::mutex> l(m);

    if (keys.empty()) return {};

    // Find the first set
    auto it = store.find(keys[0]);
    if (it == store.end())
        throw std::runtime_error("Key does not exist");
    if (it->second.type != ValueType::SET)
        throw std::runtime_error("SINTER only supported for SET types");

    std::unordered_set<std::string> result = std::get<std::unordered_set<std::string>>(it->second.data);

    // Intersect with the rest
    for (size_t i = 1; i < keys.size(); ++i) {
        auto it2 = store.find(keys[i]);
        if (it2 == store.end())
            throw std::runtime_error("Key does not exist");
        if (it2->second.type != ValueType::SET)
            throw std::runtime_error("SINTER only supported for SET types");

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
    std::unique_lock<std::mutex> l(m);

    std::unordered_set<std::string> res;
    for (const std::string &key : keys) {
        auto it = store.find(key);
        if (it == store.end())
            throw std::runtime_error("Key does not exist");

        if (it->second.type != ValueType::SET)
            throw std::runtime_error("SINTER/SUNION only supported for SET types");

        const auto& set = std::get<std::unordered_set<std::string>>(it->second.data);

        res.insert(set.begin(), set.end());
    }

    return std::vector<std::string>(res.begin(), res.end());
}