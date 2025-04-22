#include <thread>
#include <iostream>
#include "server/server.hpp"

using namespace std;

void saveDataStore() {
    while (true) {
        this_thread::sleep_for(chrono::seconds(60)); // Save every 60 seconds
        DataStore::getInstance().save();
    }
}

int main() {
    // thread saveThread(saveDataStore);
    // saveThread.detach(); // Detach the thread to run independently
    Server server;
    server.start(8080);
    return 0;
}
