#include "TCPServer.hpp"
#include "LogProcessor.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <sstream>
#include <mutex>
std::mutex cout_mutex;

void TCPServer::handle_client(SOCKET client_socket) {
    char buffer[8192];
    int bytesReceived = recv(client_socket, buffer, sizeof(buffer), 0);

    if (bytesReceived <= 0) {
        std::cerr << "ERROR: Failed to receive data from client." << std::endl;
        closesocket(client_socket);
        return;
    }

    std::string request_str(buffer, bytesReceived);
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "Received request:\n" << request_str << std::endl;
    }

    try {
        using json = nlohmann::json;
        json request = json::parse(request_str);

        std::string analysis_type = request["analysis_type"];
        std::string log_folder = request["log_folder"];

        // Parse date range
        std::string start_date = request.value("start_date", "");
        std::string end_date = request.value("end_date", "");

        std::optional<DateRange> date_range = std::nullopt;
        if (!start_date.empty() && !end_date.empty()) {
            std::tm tm_start = {}, tm_end = {};
            std::istringstream ss1(start_date), ss2(end_date);
            ss1 >> std::get_time(&tm_start, "%Y-%m-%d %H:%M:%S");
            ss2 >> std::get_time(&tm_end, "%Y-%m-%d %H:%M:%S");

            auto start_tp = std::chrono::system_clock::from_time_t(std::mktime(&tm_start));
            auto end_tp = std::chrono::system_clock::from_time_t(std::mktime(&tm_end));
            date_range = DateRange{start_tp, end_tp};
        }

        // Process logs
        LogProcessor processor(log_folder);
        std::vector<LogEntry> all_logs = processor.collect_logs(date_range);

        // Build JSON response
        json response;

        if (analysis_type == "user") {
            response["user"] = processor.analyze_by_user(all_logs);
        } else if (analysis_type == "ip") {
            response["ip"] = processor.analyze_by_ip(all_logs);
        } else if (analysis_type == "level") {
            response["level"] = processor.analyze_by_level(all_logs);
        } else {
            response["error"] = "Unknown analysis type.";
        }

        std::string response_str = response.dump();
        send(client_socket, response_str.c_str(), static_cast<int>(response_str.size()), 0);

    } catch (const std::exception& e) {
        std::cerr << "Error processing request: " << e.what() << std::endl;
    }

    closesocket(client_socket);
}
