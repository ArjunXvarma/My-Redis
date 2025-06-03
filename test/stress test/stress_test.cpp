#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>
#include <random>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

constexpr const char* HOST = "127.0.0.1";
constexpr int PORT = 8080;
constexpr int CLIENT_THREADS = 32;
constexpr int REQUESTS_PER_THREAD = 5000;
constexpr int METRICS_INTERVAL_MS = 1000;

std::atomic<int> total_requests{0};
std::atomic<long long> total_latency_ns{0};

std::string make_command(const std::string& type, int i) {
    if (type == "PING") return "*1\r\n$4\r\nPING\r\n";
    if (type == "SET") return "*3\r\n$3\r\nSET\r\n$5\r\nkey" + std::to_string(i) + "\r\n$5\r\nval" + std::to_string(i) + "\r\n";
    if (type == "GET") return "*2\r\n$3\r\nGET\r\n$5\r\nkey" + std::to_string(i) + "\r\n";
    return "";
}

void stress_worker(int id, const std::vector<std::string>& commands) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, HOST, &serv_addr.sin_addr);

    if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Client " << id << " failed to connect\n";
        return;
    }

    for (int i = 0; i < REQUESTS_PER_THREAD; ++i) {
        std::string cmd = commands[i % commands.size()];
        auto start = std::chrono::steady_clock::now();
        send(sock, cmd.c_str(), cmd.size(), 0);

        char buffer[256];
        recv(sock, buffer, sizeof(buffer), 0);
        auto end = std::chrono::steady_clock::now();

        total_latency_ns += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        total_requests++;
    }

    close(sock);
}

void metrics_logger(std::stop_token stoken, const std::string& output_csv) {
    std::ofstream csv(output_csv);
    csv << "seconds,total_requests,avg_latency_us\n";

    int last_total = 0;
    auto start = std::chrono::steady_clock::now();

    while (!stoken.stop_requested()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(METRICS_INTERVAL_MS));

        int now_total = total_requests.load();
        long long now_latency = total_latency_ns.load();
        int delta = now_total - last_total;

        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(now - start).count();

        double avg_latency_us = (now_total > 0) ? now_latency / 1000.0 / now_total : 0.0;

        std::cout << "[Time: " << int(elapsed) << "s] "
                  << "Requests: " << now_total
                  << " | Throughput: " << delta << "/s"
                  << " | Avg latency: " << avg_latency_us << " µs\n";

        csv << int(elapsed) << "," << now_total << "," << avg_latency_us << "\n";
        last_total = now_total;
    }
}

int main() {
    std::vector<std::string> commands = {
        make_command("SET", 1),
        make_command("GET", 1),
        make_command("PING", 1)
    };

    std::jthread metrics_thread(metrics_logger, "metrics.csv");

    std::vector<std::thread> clients;
    for (int i = 0; i < CLIENT_THREADS; ++i) {
        clients.emplace_back(stress_worker, i, std::cref(commands));
    }

    for (auto& t : clients) t.join();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));  // Let metrics catch final data
}

