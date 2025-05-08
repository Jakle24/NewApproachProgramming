#pragma once
#include <winsock2.h>
#include <string>

class TCPClient {
public:
    TCPClient(const std::string& server_ip, int port);
    ~TCPClient();
    void start();

private:
    std::string server_ip_;
    int port_;
    SOCKET sock_;
};