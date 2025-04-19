#include <sstream>
#include <vector>
#include <sys/socket.h> // For recv and send
#include <unistd.h>     // For close
#include <cstring>      // For memset (if needed)
#include "server/clientHandler.hpp"

using namespace std;

bool ClientHandler::handle(int clientSocket) {
    char buffer[1024] = {0};
    ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived <= 0) return false;

    string input(buffer);
    istringstream iss(input);
    vector<string> tokens;
    string token;
    while (iss >> token) tokens.push_back(token);

    CommandDispatcher dispatcher;
    string response = dispatcher.dispatch(tokens);
    send(clientSocket, response.c_str(), response.length(), 0);
    return true;
}
