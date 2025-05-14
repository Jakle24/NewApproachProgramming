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

TCPServer::TCPServer(int port) : port(port), running(false) {}

TCPServer::~TCPServer() {
    stop();
    WSACleanup();
}

bool TCPServer::initialize_winsock() {
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
}

void TCPServer::start() {
    if (!initialize_winsock()) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return;
    }
    
    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET) {
        std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
        return;
    }
    
    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = INADDR_ANY;
    service.sin_port = htons(port);
    
    if (bind(server_socket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
        std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
        closesocket(server_socket);
        return;
    }
    
    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(server_socket);
        return;
    }
    
    std::cout << "Server started. Listening on port " << port << "..." << std::endl;
    
    running = true;
    std::vector<std::thread> client_threads;
    
    while (running) {
        SOCKET client_socket = accept(server_socket, NULL, NULL);
        if (client_socket == INVALID_SOCKET) {
            if (running) {
                std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
            }
            continue;
        }
        
        std::cout << "Client connected." << std::endl;
        client_threads.push_back(std::thread(&TCPServer::handle_client, this, client_socket));
    }
    
    for (auto& thread : client_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void TCPServer::stop() {
    running = false;
    closesocket(server_socket);
}

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
        std::string folder = request["log_folder"];

        {   // debug:
            std::lock_guard<std::mutex> lk(cout_mutex);
            std::cout << "→ Log folder from client: " << folder << "\n";
        }

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

        // Construct the processor with the folder:
        LogProcessor processor(folder);

        // Now call the right analysis:
        json result;
        if (analysis_type == "user") {
            result = processor.analyze_by_user(date_range);
        } else if (analysis_type == "ip") {
            result = processor.analyze_by_ip(date_range);
        } else if (analysis_type == "level") {
            result = processor.analyze_by_level(date_range);
        } else {
            result["error"] = "Unknown analysis type";
        }
        
        std::string response = result.dump(4);
        send(client_socket, response.c_str(), response.length(), 0);
        
    } catch (const std::exception& e) {
        std::string error_msg = "Error processing request: ";
        error_msg += e.what();
        
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cerr << error_msg << std::endl;
        }
        
        nlohmann::json error_response = {{"error", error_msg}};
        std::string response = error_response.dump();
        send(client_socket, response.c_str(), response.length(), 0);
    }
    
    closesocket(client_socket);
}
