#include "TCPClient.hpp"
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>

using json = nlohmann::json;

TCPClient::TCPClient(const std::string& server_ip, int server_port) 
    : server_ip(server_ip), server_port(server_port), client_socket(INVALID_SOCKET) {
    initialize_winsock();
}

TCPClient::~TCPClient() {
    if (client_socket != INVALID_SOCKET) {
        closesocket(client_socket);
    }
    WSACleanup();
}

bool TCPClient::initialize_winsock() {
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
}

bool TCPClient::connect_to_server() {
    if (client_socket != INVALID_SOCKET) {
        closesocket(client_socket);
    }
    
    client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket == INVALID_SOCKET) {
        std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
        return false;
    }
    
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_port = htons(server_port);
    
    inet_pton(AF_INET, server_ip.c_str(), &clientService.sin_addr.s_addr);
    
    if (connect(client_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to server: " << WSAGetLastError() << std::endl;
        closesocket(client_socket);
        client_socket = INVALID_SOCKET;
        return false;
    }
    
    return true;
}

nlohmann::json TCPClient::send_request(const nlohmann::json& request) {
    if (!connect_to_server()) {
        return {{"error", "Failed to connect to server"}};
    }
    
    std::string request_str = request.dump();
    
    if (send(client_socket, request_str.c_str(), request_str.length(), 0) == SOCKET_ERROR) {
        std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
        return {{"error", "Failed to send request to server"}};
    }
    
    // Receive response
    std::vector<char> buffer(8192);
    int bytes_received = recv(client_socket, buffer.data(), buffer.size(), 0);
    
    if (bytes_received == SOCKET_ERROR) {
        std::cerr << "Receive failed: " << WSAGetLastError() << std::endl;
        return {{"error", "Failed to receive response from server"}};
    }
    
    std::string response_str(buffer.data(), bytes_received);
    
    try {
        nlohmann::json response = nlohmann::json::parse(response_str);
        return response;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing response: " << e.what() << std::endl;
        return {{"error", "Failed to parse server response"}};
    }
}
