#include "persistence/persistenceManager.hpp"
#include <fstream>
#include <iostream>
#include <future>
#include <sstream>
#include <mutex>

char valueTypeChar(ValueType type) {
    switch (type) {
        case ValueType::STRING: return 'S';
        case ValueType::LIST:   return 'L';
        case ValueType::SET:    return 'T'; // 'S' was used for String
        case ValueType::HASH:   return 'H';
        default:                return '?';
    }
}

std::unordered_map<std::string, Value> PersistenceManager::load() {
    extern std::string dumpFileName;
    std::unordered_map<std::string, Value> data;
    std::ifstream file(dumpFileName);

    std::cout << "[Persistence] Loading data from '" << dumpFileName << "'..." << std::endl;

    if (!file.is_open()) {
        std::cout << "[Persistence] No file found at '" << dumpFileName << "'. Starting fresh." << std::endl;
        return data;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        char typeChar;
        std::string key;
        iss >> typeChar >> key;

        switch (typeChar) {
            case 'S': {
                std::string value;
                iss >> std::ws;
                std::getline(iss, value);
                data[key] = Value{ValueType::STRING, value};
                break;
            }
            case 'L': {
                std::string valuesStr;
                std::getline(iss >> std::ws, valuesStr);
                std::deque<std::string> list;
                std::istringstream valStream(valuesStr);
                std::string item;
                while (std::getline(valStream, item, ',')) {
                    list.push_back(item);
                }
                data[key] = Value{ValueType::LIST, list};
                break;
            }
            case 'T': { // Set
                std::string valuesStr;
                std::getline(iss >> std::ws, valuesStr);
                std::unordered_set<std::string> s;
                std::istringstream valStream(valuesStr);
                std::string item;
                while (std::getline(valStream, item, ',')) {
                    s.insert(item);
                }
                data[key] = Value{ValueType::SET, s};
                break;
            }
            case 'H': {
                std::string valuesStr;
                std::getline(iss >> std::ws, valuesStr);
                std::unordered_map<std::string, std::string> h;
                std::istringstream valStream(valuesStr);
                std::string field, value;
                while (std::getline(valStream, field, ',') && std::getline(valStream, value, ',')) {
                    h[field] = value;
                }
                data[key] = Value{ValueType::HASH, h};
                break;
            }
            default:
            std::cerr << "[Persistence] Unknown type '" << typeChar << "' in line: " << line << std::endl;
            break;
        }
    }
    
    file.close();
    return data;
}

void PersistenceManager::save(const std::unordered_map<std::string, Value>& data) {
    extern ThreadPool globalThreadPool;
    extern std::string dumpFileName;

    globalThreadPool.enqueue([data]() {
        std::ofstream file(dumpFileName, std::ios::trunc);
        if (!file.is_open()) {
            std::cerr << "[Persistence] Failed to open file: " << dumpFileName << std::endl;
            return;
        }

        std::mutex fileMutex;
        std::vector<std::future<void>> futures;
        size_t batchSize = 50;
        auto it = data.begin();

        while (it != data.end()) {
            std::unordered_map<std::string, Value> batch;
            for (size_t i = 0; i < batchSize && it != data.end(); ++i, ++it) {
                batch[it->first] = it->second;
            }

            futures.emplace_back(std::async(std::launch::async, [&file, &fileMutex, batch]() {
                std::ostringstream oss;
                for (const auto& [key, value] : batch) {
                    oss << valueTypeChar(value.type) << " " << key << " ";

                    switch (value.type) {
                        case ValueType::STRING:
                            oss << std::get<std::string>(value.data);
                            break;
                        case ValueType::LIST: {
                            const auto& list = std::get<std::deque<std::string>>(value.data);
                            for (auto it = list.begin(); it != list.end(); ++it) {
                                oss << *it;
                                if (std::next(it) != list.end()) oss << ",";
                            }
                            break;
                        }
                        case ValueType::SET: {
                            const auto& s = std::get<std::unordered_set<std::string>>(value.data);
                            size_t count = 0;
                            for (const auto& elem : s) {
                                oss << elem;
                                if (++count < s.size()) oss << ",";
                            }
                            break;
                        }
                        case ValueType::HASH: {
                            const auto& h = std::get<std::unordered_map<std::string, std::string>>(value.data);
                            size_t count = 0;
                            for (const auto& [field, val] : h) {
                                oss << field << "," << val;
                                if (++count < h.size()) oss << ",";
                            }
                            break;
                        }
                    }
                    oss << "\n";
                }

                std::lock_guard<std::mutex> lock(fileMutex);
                file << oss.str();
            }));
        }

        for (auto& f : futures) f.get();

        std::cout << "[Persistence] Data saved to " << dumpFileName << std::endl;
        file.close();
    });
}
