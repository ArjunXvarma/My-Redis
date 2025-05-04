#include "persistence/persistenceManager.hpp"
#include <fstream>
#include <iostream>

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
    ThreadPool globalThreadPool;
    extern std::string dumpFileName;

    globalThreadPool.enqueue([data]() {
        std::ofstream file(dumpFileName, std::ios::trunc);
        if (!file.is_open()) {
            std::cerr << "[Persistence] Failed to open file: " << dumpFileName << std::endl;
            return;
        }
    
        for (const auto& [key, value] : data) {
            file << key << " " << value << std::endl;
        }
    
        std::cout << "[Persistence] Data saved to " << dumpFileName << std::endl;
        file.close(); 
    });
}
