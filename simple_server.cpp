#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <nlohmann/json.hpp>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "src/LogProcessor.hpp"

#pragma comment(lib, "ws2_32.lib")

void handleClient(SOCKET clientSocket) {
    char buffer[4096];
    int bytesReceived = recv(clientSocket, buffer, 4096, 0);
    if (bytesReceived > 0) {
        // Parse the request
        buffer[bytesReceived] = '\0';
        nlohmann::json request = nlohmann::json::parse(buffer);
        std::string log_folder = request.value("log_folder", "TestLog");
        std::string analysis_type = request.value("analysis_type", "user");
        
        // Process the request
        LogProcessor processor(log_folder);
        nlohmann::json response;
        
        if (analysis_type == "user") {
            response = processor.analyze_by_user();
        } 
        else if (analysis_type == "ip") {
            response = processor.analyze_by_ip();
        }
        else if (analysis_type == "level") {
            response = processor.analyze_by_level();
        }
        
        // Send the response
        std::string responseStr = response.dump();
        send(clientSocket, responseStr.c_str(), responseStr.length(), 0);
    }
    closesocket(clientSocket);
}

int main() {
    // Initialize Winsock
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);
    if (WSAStartup(ver, &wsData) != 0) {
        std::cerr << "Winsock startup failed\n";
        return 1;
    }
    
    // Create socket
    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }
    
    // Bind socket
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(8080);
    hint.sin_addr.S_un.S_addr = INADDR_ANY;
    
    if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR) {
        std::cerr << "Socket binding failed\n";
        closesocket(listening);
        WSACleanup();
        return 1;
    }
    
    // Listen for connections
    if (listen(listening, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Socket listening failed\n";
        closesocket(listening);
        WSACleanup();
        return 1;
    }
    
    std::cout << "Server started on port 8080\n";
    
    // Vector to track thread handles
    std::vector<std::thread> threads;
    
    // Accept connections
    while (true) {
        sockaddr_in client;
        int clientSize = sizeof(client);
        SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
        
        if (clientSocket != INVALID_SOCKET) {
            std::cout << "Client connected\n";
            // Create thread to handle client
            threads.push_back(std::thread(handleClient, clientSocket));
            // Detach thread so it runs independently
            threads.back().detach();
        }
    }
    
    // Clean up
    closesocket(listening);
    WSACleanup();
    return 0;
}