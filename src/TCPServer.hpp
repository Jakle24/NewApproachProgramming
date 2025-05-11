#pragma once
#include <winsock2.h>

class TCPServer {
public:
    TCPServer(int port);
    ~TCPServer();
    void start();
    void stop();

private:
    int port_;
    SOCKET server_socket_;
    void handle_client(SOCKET client_socket);
};
