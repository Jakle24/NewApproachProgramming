#pragma once
#include <winsock2.h>
#include <string>
#include <nlohmann/json.hpp> // JSON support

class TCPClient {
public:
    TCPClient(const std::string& server_ip, int port);
    ~TCPClient();

    void start(); // Main client loop

private:
    std::string server_ip_;
    int port_;
    SOCKET sock_;

    void send_analysis_request();
    void receive_and_display_response();
};
