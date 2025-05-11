#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

class TCPClient {
public:
    TCPClient(const std::string& server_ip = "127.0.0.1", int server_port = 8080);
    ~TCPClient();
    
    nlohmann::json send_request(const nlohmann::json& request);
    
private:
    std::string server_ip;
    int server_port;
    SOCKET client_socket;
    
    bool initialize_winsock();
    bool connect_to_server();
};
