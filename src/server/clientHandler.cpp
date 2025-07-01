#include "server/clientHandler.hpp"
#include "utils/GlobalThreadPool.hpp"
#include "protocol/RESPparser.hpp"
#include "protocol/RESPencoder.hpp"
#include <mutex>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <algorithm>
#include <sys/socket.h>
#include <cstring>

extern ThreadPool globalThreadPool;

ClientHandler::ClientHandler(int fd) : socket_fd(fd) {}

bool ClientHandler::handle() {
    std::string input;
    if (!readFromSocket(input)) return false;

    dispatchToThreadPool(input);

    return true;
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

void ClientHandler::dispatchToThreadPool(const std::string& input) {
    auto self = shared_from_this();

    globalThreadPool.enqueue([self, input]() {
        self->handleCommand(input);
    });
}

void ClientHandler::handleCommand(const std::string& input) {
    auto tokens = RESPParser::parse(input);
    if (tokens.empty()) {
        sendError("Invalid or incomplete command");
        return;
    }

    std::string cmd = tokens[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

    std::string response;

    if (txn.in_transaction && cmd != "EXEC" && cmd != "DISCARD" && cmd != "MULTI") {
        txn.queued_commands.push_back(tokens);
        response = "+QUEUED";
    } else {
        CommandDispatcher dispatcher;
        response = dispatcher.dispatch(tokens, txn);
    }

    sendResponse(response);
}

void ClientHandler::sendError(const std::string& msg) {
    std::string error = RESPEncoder::encodeError(msg);
    std::lock_guard<std::mutex> lock(write_mutex);
    send(socket_fd, error.c_str(), error.size(), 0);
}

void ClientHandler::sendResponse(const std::string& response) {
    std::string encoded = RESPEncoder::encodeBulkString(response);
    std::lock_guard<std::mutex> lock(write_mutex);

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
