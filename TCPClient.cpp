#include "TCPClient.hpp"
#include <iostream>
#include <sstream>
#include <fstream>

using json = nlohmann::json;

TCPClient::TCPClient(const std::string& server_ip, int port)
    : server_ip_(server_ip), port_(port), sock_(INVALID_SOCKET) {}

TCPClient::~TCPClient() {
    if (sock_ != INVALID_SOCKET) closesocket(sock_);
    WSACleanup();
}

void TCPClient::start() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return;
    }

    sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_ == INVALID_SOCKET) {
        std::cerr << "Socket creation failed.\n";
        WSACleanup();
        return;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    server_addr.sin_addr.s_addr = inet_addr(server_ip_.c_str());

    if (connect(sock_, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed.\n";
        closesocket(sock_);
        WSACleanup();
        return;
    }

    std::cout << "Connected to server.\n";

    send_analysis_request();
    receive_and_display_response();

    closesocket(sock_);
    WSACleanup();
}

void TCPClient::send_analysis_request() {
    std::string analysis_type;
    std::cout << "Choose analysis [user, ip, level]: ";
    std::getline(std::cin, analysis_type);

    std::string start_date, end_date;
    std::cout << "Optional start date (YYYY-MM-DD HH:MM:SS): ";
    std::getline(std::cin, start_date);
    std::cout << "Optional end date (YYYY-MM-DD HH:MM:SS): ";
    std::getline(std::cin, end_date);

    std::string log_folder;
    std::cout << "Enter log folder path (e.g., LogFiles/Client1): ";
    std::getline(std::cin, log_folder);

    json request_json;
    request_json["command"] = "analyze";
    request_json["analysis_type"] = analysis_type;
    request_json["start_date"] = start_date;
    request_json["end_date"] = end_date;
    request_json["log_folder"] = log_folder;

    std::string msg = request_json.dump();
    send(sock_, msg.c_str(), static_cast<int>(msg.size()), 0);
}

void TCPClient::receive_and_display_response() {
    char buffer[8192];
    int bytes_received = recv(sock_, buffer, sizeof(buffer), 0);
    if (bytes_received <= 0) {
        std::cerr << "Failed to receive response from server.\n";
        return;
    }

    std::string response(buffer, bytes_received);

    try {
        json result = json::parse(response);

        std::cout << "\n--- Analysis Results ---\n";
        for (const auto& [category, stats] : result.items()) {
            std::cout << "[" << category << "]\n";
            for (const auto& [key, count] : stats.items()) {
                std::cout << "  " << key << ": " << count << "\n";
            }
        }
        std::cout << "--------------------------\n";
    } catch (const std::exception& e) {
        std::cerr << "Error parsing server response: " << e.what() << "\n";
    }
}
