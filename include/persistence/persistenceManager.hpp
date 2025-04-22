#pragma once
#include <unordered_map>
#include <string>

using namespace std;

class PersistenceManager {
public:
    static void save(const unordered_map<string, string>& store);
    static unordered_map<string, string> load();
};