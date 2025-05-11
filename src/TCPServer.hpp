#pragma once
#include <string>
#include <functional>
#include <atomic>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

class TCPServer {
public:
    TCPServer(int port = 8080);
    ~TCPServer();
    
    void start();
    void stop();
    
private:
    int port;
    SOCKET server_socket;
    std::atomic<bool> running;
    
    void handle_client(SOCKET client_socket);
    bool initialize_winsock();
};
