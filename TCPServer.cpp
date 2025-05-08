#include "TCPServer.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

std::mutex cout_mutex;

TCPServer::TCPServer(int port) : port_(port), server_socket_(INVALID_SOCKET) {}

TCPServer::~TCPServer() {
    if (server_socket_ != INVALID_SOCKET) closesocket(server_socket_);
    WSACleanup();
}

void TCPServer::handle_client(SOCKET client_socket) {
    char buffer[1024];
    int bytesReceived;
    while ((bytesReceived = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "Received: " << std::string(buffer, bytesReceived) << std::endl;
        send(client_socket, buffer, bytesReceived, 0);
    }
    closesocket(client_socket);
}

void TCPServer::start() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    server_socket_ = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_);

    bind(server_socket_, (sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_socket_, SOMAXCONN);

    std::vector<std::thread> threads;

    std::cout << "Server listening on port " << port_ << "..." << std::endl;

    while (true) {
        SOCKET client_socket = accept(server_socket_, nullptr, nullptr);
        if (client_socket != INVALID_SOCKET) {
            threads.emplace_back(&TCPServer::handle_client, this, client_socket);
        }
    }

    for (auto& t : threads) t.join();
}