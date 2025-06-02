#include <sstream>
#include <iostream>
#include <vector>
#include <sys/socket.h> 
#include <unistd.h>
#include <cstring>
#include "server/clientHandler.hpp"

void ClientHandler::sendError(const std::string& msg) {
    std::string error = RESPEncoder::encodeError(msg);
    send(socket_fd, error.c_str(), error.size(), 0);
}


void ClientHandler::sendResponse(const std::string& response) {
    std::future<std::string> futureEncoded = 
        globalThreadPool.enqueue(RESPEncoder::encodeBulkString, response);
    std::string encoded = futureEncoded.get();

    size_t totalSent = 0;
    while (totalSent < encoded.size()) {
        ssize_t sent = send(socket_fd, encoded.c_str() + totalSent, encoded.size() - totalSent, 0);
        if (sent <= 0) {
            perror("send failed");
            break;
        }
        totalSent += sent;
    }
}

std::vector<std::string> ClientHandler::parseInput(const std::string& input) {
    std::future<std::vector<std::string>> futureTokens = 
        globalThreadPool.enqueue(RESPParser::parse, input);
    return futureTokens.get();
}

std::string ClientHandler::processCommand(const std::vector<std::string>& tokens) {
    std::string cmd = tokens[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

    if (txn.in_transaction && cmd != "EXEC" && cmd != "DISCARD" && cmd != "MULTI") {
        std::ostringstream oss;
        for (const auto& t : tokens) {
            oss << t << " ";
        }
        txn.queued_commands.push_back(tokens);
        return "+QUEUED";
    }

    CommandDispatcher dispatcher;
    return dispatcher.dispatch(tokens, txn);  // pass txn by ref
}

bool ClientHandler::readFromSocket(std::string& input) {
    char buffer[1024] = {0};
    ssize_t bytesReceived = recv(socket_fd, buffer, sizeof(buffer) - 1, 0);

    if (bytesReceived == 0) {
        std::cout << "Client disconnected." << std::endl;
        return false;
    } else if (bytesReceived < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) return true;
        perror("recv");
        return false;
    }

    buffer[bytesReceived] = '\0';
    input = buffer;
    return true;
}

bool ClientHandler::handle() {
    std::string input;
    if (!readFromSocket(input)) return false;

    std::vector<std::string> tokens = parseInput(input);
    if (tokens.empty()) {
        sendError("Invalid or incomplete command");
        return true;
    }

    std::string response = processCommand(tokens);
    sendResponse(response);
    return true;
}