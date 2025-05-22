#include "persistence/persistenceManager.hpp"
#include <fstream>
#include <iostream>
#include <future>
#include <sstream>
#include <mutex>

namespace {
    char typeToChar(ValueType type) {
        switch (type) {
            case ValueType::STRING: return 'S';
            case ValueType::LIST: return 'L';
            case ValueType::SET: return 'T'; 
            case ValueType::HASH: return 'H';
            default: return '?';
        }
    }
    
    ValueType charToType(char c) {
        switch (c) {
            case 'S': return ValueType::STRING;
            case 'L': return ValueType::LIST;
            case 'T': return ValueType::SET;
            case 'H': return ValueType::HASH;
            default: throw std::runtime_error("Unknown value type char");
        }
    }
    
    std::string join(const std::vector<std::string>& parts, const std::string& sep) {
        std::ostringstream oss;
        for (size_t i = 0; i < parts.size(); ++i) {
            oss << parts[i];
            if (i + 1 < parts.size()) oss << sep;
        }
        return oss.str();
    }
    
    std::string serializeValue(const Value& value) {
        switch (value.type) {
            case ValueType::STRING:
                return std::get<std::string>(value.data);
    
            case ValueType::LIST: {
                const auto& list = std::get<std::deque<std::string>>(value.data);
                return join({list.begin(), list.end()}, ",");
            }
    
            case ValueType::SET: {
                const auto& s = std::get<std::unordered_set<std::string>>(value.data);
                return join({s.begin(), s.end()}, ",");
            }
    
            case ValueType::HASH: {
                const auto& h = std::get<std::unordered_map<std::string, std::string>>(value.data);
                std::vector<std::string> parts;
                for (const auto& [k, v] : h) {
                    parts.push_back(k);
                    parts.push_back(v);
                }
                return join(parts, ",");
            }
    
            default:
                return "";
        }
    }
    
    std::optional<std::pair<std::string, Value>> deserializeLine(const std::string& line) {
        std::istringstream iss(line);
        char typeChar;
        std::string key;
    
        if (!(iss >> typeChar >> key)) return std::nullopt;
    
        std::string rest;
        std::getline(iss >> std::ws, rest);
    
        ValueType type = charToType(typeChar);
    
        switch (type) {
            case ValueType::STRING:
                return {{key, Value{type, rest}}};
    
            case ValueType::LIST: {
                std::deque<std::string> list;
                std::istringstream valStream(rest);
                std::string item;
                while (std::getline(valStream, item, ',')) list.push_back(item);
                return {{key, Value{type, list}}};
            }
    
            case ValueType::SET: {
                std::unordered_set<std::string> s;
                std::istringstream valStream(rest);
                std::string item;
                while (std::getline(valStream, item, ',')) s.insert(item);
                return {{key, Value{type, s}}};
            }
    
            case ValueType::HASH: {
                std::unordered_map<std::string, std::string> h;
                std::istringstream valStream(rest);
                std::string field, val;
                while (std::getline(valStream, field, ',') && std::getline(valStream, val, ',')) {
                    h[field] = val;
                }
                return {{key, Value{type, h}}};
            }
    
            default:
                return std::nullopt;
        }
    }
} 

std::unordered_map<std::string, Value> PersistenceManager::load() {
    extern std::string dumpFileName;
    std::unordered_map<std::string, Value> data;
    std::ifstream file(dumpFileName);

    std::cout << "[Persistence] Loading data from '" << dumpFileName << "'..." << std::endl;

    if (!file.is_open()) {
        std::cout << "[Persistence] No file found. Starting fresh." << std::endl;
        return data;
    }

    std::string line;
    while (std::getline(file, line)) {
        auto kv = deserializeLine(line);
        
        if (kv.has_value()) data.insert(*kv);
        else std::cerr << "[Persistence] Failed to parse line: " << line << std::endl;
    }

    return data;
}

void PersistenceManager::save(const std::unordered_map<std::string, Value>& data) {
    extern ThreadPool globalThreadPool;
    extern std::string dumpFileName;

    globalThreadPool.enqueue([data]() {
        std::ofstream file(dumpFileName, std::ios::trunc);
        if (!file.is_open()) {
            std::cerr << "[Persistence] Failed to open file for writing." << std::endl;
            return;
        }

        std::mutex fileMutex;
        std::vector<std::future<void>> futures;

        const size_t batchSize = 50;
        auto it = data.begin();

        while (it != data.end()) {
            std::unordered_map<std::string, Value> batch;
            for (size_t i = 0; i < batchSize && it != data.end(); ++i, ++it) 
                batch[it->first] = it->second;

            futures.emplace_back(std::async(std::launch::async, [&file, &fileMutex, batch]() {
                std::ostringstream oss;
                for (const auto& [key, value] : batch) 
                    oss << typeToChar(value.type) << " " << key << " " << serializeValue(value) << "\n";

                std::lock_guard<std::mutex> lock(fileMutex);
                file << oss.str();
            }));
        }

        for (auto& f : futures) f.get();

        std::cout << "[Persistence] Data saved to " << dumpFileName << std::endl;
        file.close();
    });
}
