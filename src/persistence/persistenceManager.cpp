#include "persistence/persistenceManager.hpp"
#include <fstream>
#include <iostream>

unordered_map<string, string> PersistenceManager::load() {
    extern string dumpFileName;
    
    unordered_map<string, string> data;
    ifstream file(dumpFileName);
    cout << "[Persistence] Loading data from '" << dumpFileName << "'..." << endl;
    
    if (!file.is_open()) {
        cout << "[Persistence] No file found at '" << dumpFileName << "'. Starting fresh." << endl;
        return data;
    }

    string key, value;
    while (file >> key >> value) {
        data[key] = value;
    }

    file.close();
    return data;
}

void PersistenceManager::save(const unordered_map<string, string>& data) {
    extern string dumpFileName;
    ofstream file(dumpFileName, ios::trunc);
    if (!file.is_open()) {
        cerr << "[Persistence] Failed to open file: " << dumpFileName << endl;
        return;
    }

    for (const auto& [key, value] : data) {
        file << key << " " << value << endl;
    }

    cout << "[Persistence] Data saved to " << dumpFileName << endl;
    file.close();
}
