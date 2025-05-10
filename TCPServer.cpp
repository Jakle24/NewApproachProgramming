#include "TCPServer.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <algorithm>

std::mutex cout_mutex;
std::atomic<bool> server_running(true); // Atomic flag for graceful shutdown

TCPServer::TCPServer(int port) : port_(port), server_socket_(INVALID_SOCKET) {}

TCPServer::~TCPServer() {
    if (server_socket_ != INVALID_SOCKET) closesocket(server_socket_);
    WSACleanup();
}

void TCPServer::handle_client(SOCKET client_socket) {
    char buffer[1024];
    int bytesReceived;

    while ((bytesReceived = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "Received: " << std::string(buffer, bytesReceived) << std::endl;
        }

        // Echo the message back to the client
        if (send(client_socket, buffer, bytesReceived, 0) == SOCKET_ERROR) {
            std::cerr << "ERROR: Failed to send data to client." << std::endl;
            break;
        }
    }

    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "ERROR: Failed to receive data from client." << std::endl;
    }

    closesocket(client_socket);
}

void TCPServer::start() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "ERROR: WSAStartup failed." << std::endl;
        return;
    }

    server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_ == INVALID_SOCKET) {
        std::cerr << "ERROR: Failed to create socket." << std::endl;
        WSACleanup();
        return;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_);

    if (bind(server_socket_, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "ERROR: Failed to bind socket." << std::endl;
        closesocket(server_socket_);
        WSACleanup();
        return;
    }

    if (listen(server_socket_, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "ERROR: Failed to listen on socket." << std::endl;
        closesocket(server_socket_);
        WSACleanup();
        return;
    }

    std::vector<std::thread> threads;

    std::cout << "Server listening on port " << port_ << "..." << std::endl;

    while (server_running) {
        SOCKET client_socket = accept(server_socket_, nullptr, nullptr);
        if (client_socket == INVALID_SOCKET) {
            if (server_running) {
                std::cerr << "ERROR: Failed to accept client connection." << std::endl;
            }
            break;
        }

        // Launch a new thread to handle the client
        threads.emplace_back(&TCPServer::handle_client, this, client_socket);

        // Clean up finished threads
        threads.erase(std::remove_if(threads.begin(), threads.end(),
                                     [](std::thread& t) {
                                         if (t.joinable()) {
                                             t.join();
                                             return true;
                                         }
                                         return false;
                                     }),
                      threads.end());
    }

    // Join all remaining threads before exiting
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    std::cout << "Server shutting down..." << std::endl;
}

void TCPServer::stop() {
    server_running = false;
    closesocket(server_socket_); // Force accept() to return
}