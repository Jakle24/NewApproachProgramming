#include "TCPClient.hpp"
#include <iostream>

TCPClient::TCPClient(const std::string& server_ip, int port)
    : server_ip_(server_ip), port_(port), sock_(INVALID_SOCKET) {}

TCPClient::~TCPClient() {
    if (sock_ != INVALID_SOCKET) closesocket(sock_);
    WSACleanup();
}

void TCPClient::start() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    sock_ = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    server_addr.sin_addr.s_addr = inet_addr(server_ip_.c_str());

    if (connect(sock_, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed." << std::endl;
        closesocket(sock_);
        WSACleanup();
        return;
    }

    std::string msg;
    char buffer[1024];
    while (true) {
        std::cout << "Enter message: ";
        std::getline(std::cin, msg);
        if (msg == "exit") break;
        send(sock_, msg.c_str(), msg.size(), 0);
        int bytesReceived = recv(sock_, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            std::cout << "Server: " << std::string(buffer, bytesReceived) << std::endl;
        }
    }
    closesocket(sock_);
    WSACleanup();
}