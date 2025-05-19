#include "persistence/persistenceManager.hpp"
#include <fstream>
#include <iostream>
#include <future>
#include <sstream>
#include <mutex>

std::unordered_map<std::string, std::string> PersistenceManager::load() {
    extern std::string dumpFileName;
    
    std::unordered_map<std::string, std::string> data;
    std::ifstream file(dumpFileName);
    std::cout << "[Persistence] Loading data from '" << dumpFileName << "'..." << std::endl;
    
    if (!file.is_open()) {
        std::cout << "[Persistence] No file found at '" << dumpFileName << "'. Starting fresh." << std::endl;
        return data;
    }

    std::string key, value;
    while (file >> key >> value) {
        data[key] = value;
    }

    file.close();
    return data;
}

void PersistenceManager::save(const std::unordered_map<std::string, std::string>& data) {
    extern ThreadPool globalThreadPool;
    extern std::string dumpFileName;

    globalThreadPool.enqueue(std::function<void()>([data]() {
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
            std::unordered_map<std::string, std::string> batch;
            for (size_t i = 0; i < batchSize && it != data.end(); ++i, ++it) {
                batch[it->first] = it->second;
            }

            futures.emplace_back(std::async(std::launch::async, [&file, &fileMutex, batch]() {
                std::ostringstream oss;
                for (const auto& [key, value] : batch) {
                    oss << key << " " << value << "\n";
                }
                std::lock_guard<std::mutex> lock(fileMutex);
                file << oss.str();
            }));
        }

        for (auto& f : futures) f.get();

        std::cout << "[Persistence] Data saved to " << dumpFileName << std::endl;
        file.close();
    }));
}
