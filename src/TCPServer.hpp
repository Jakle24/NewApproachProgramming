#pragma once
#include <string>
#include <functional>
#include <atomic>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

/**
 * @class TCPServer
 * @brief Multi-threaded server implementation for handling client connections
 * 
 * Provides functionality to accept multiple client connections concurrently
 * and process log analysis requests using worker threads.
 */
class TCPServer {
public:
    /**
     * @brief Constructs a server that listens on the specified port
     * @param port TCP port number for listening (default 8080)
     */
    TCPServer(int port = 8080);
    
    /**
     * @brief Destructor that ensures proper cleanup of socket resources
     */
    ~TCPServer();
    
    /**
     * @brief Starts the server and begins accepting client connections
     * 
     * Initializes Winsock, creates a socket, binds to the configured port,
     * and enters a loop to accept and handle client connections.
     * Each client is processed in a separate thread.
     */
    void start();
    
    /**
     * @brief Stops the server and terminates all connections
     * 
     * Signals worker threads to complete and closes the server socket.
     */
    void stop();
    
private:
    int port;                // TCP port for server to listen on
    SOCKET server_socket;    // Main server socket for accepting connections
    std::atomic<bool> running;  // Control flag for the main server loop
    
    /**
     * @brief Handles communication with a connected client
     * @param client_socket Socket for the connected client
     * 
     * Processes client requests for log analysis and sends back results.
     * Runs in its own thread to allow concurrent client handling.
     */
    void handle_client(SOCKET client_socket);
    
    /**
     * @brief Initializes the Windows socket library
     * @return True if initialization succeeded, false otherwise
     */
    bool initialize_winsock();
};
