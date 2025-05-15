#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

/**
 * @class TCPClient
 * @brief Client implementation for sending requests to the log analysis server
 * 
 * Provides functionality to connect to the server, send log analysis requests,
 * and receive and parse the results.
 */
class TCPClient {
public:
void start();
    /**
     * void
     * @brief Constructs a client that connects to the specified server
     * @param server_ip IP address of the server (default: localhost)
     * @param server_port TCP port of the server (default: 8080)
     */
    TCPClient(const std::string& server_ip = "127.0.0.1", int server_port = 8080);
    
    /**
     * @brief Destructor that ensures proper cleanup of socket resources
     */
    ~TCPClient();
    
    /**
     * @brief Sends a request to the server and returns the response
     * @param request JSON object containing the analysis request parameters
     * @return JSON object with the analysis results or error message
     * 
     * Establishes a connection to the server, sends the serialized request,
     * waits for and receives the response, then parses and returns it.
     */
    nlohmann::json send_request(const nlohmann::json& request);
    
private:
    std::string server_ip;    // IP address of the server to connect to
    int server_port;          // TCP port of the server to connect to
    SOCKET client_socket;     // Socket for communicating with the server
    
    /**
     * @brief Initializes the Windows socket library
     * @return True if initialization succeeded, false otherwise
     */
    bool initialize_winsock();
    
    /**
     * @brief Establishes a connection to the server
     * @return True if connection succeeded, false otherwise
     */
    bool connect_to_server();
};
