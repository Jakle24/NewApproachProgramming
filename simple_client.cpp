#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main(int argc, char* argv[]) {
    // Parse command line arguments
    if (argc < 5) {
        std::cout << "Usage: simple_client --log-folder <folder> --analysis <type>\n";
        return 1;
    }
    
    std::string log_folder;
    std::string analysis_type;
    
    for (int i = 1; i < argc; i += 2) {
        std::string arg = argv[i];
        if (arg == "--log-folder" && i + 1 < argc) {
            log_folder = argv[i + 1];
        } 
        else if (arg == "--analysis" && i + 1 < argc) {
            analysis_type = argv[i + 1];
        }
    }
    
    if (log_folder.empty() || analysis_type.empty()) {
        std::cout << "Missing required parameters\n";
        return 1;
    }
    
    // Initialize Winsock
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);
    if (WSAStartup(ver, &wsData) != 0) {
        std::cerr << "Winsock startup failed\n";
        return 1;
    }
    
    // Create socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }
    
    // Set up server address
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &hint.sin_addr);
    
    // Connect to server
    if (connect(sock, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR) {
        std::cerr << "Connection to server failed\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    
    std::cout << "Connected to server\n";
    
    // Create JSON request
    nlohmann::json request;
    request["log_folder"] = log_folder;
    request["analysis_type"] = analysis_type;
    
    // Send request
    std::string requestStr = request.dump();
    send(sock, requestStr.c_str(), requestStr.size(), 0);
    
    // Receive response
    char buffer[4096];
    int bytesReceived = recv(sock, buffer, 4096, 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        
        // Parse and display results
        nlohmann::json response = nlohmann::json::parse(buffer);
        std::cout << "\n=== Analysis Results ===\n\n";
        std::cout << response.dump(4) << std::endl;  // Pretty-print with 4-space indent
    }
    
    // Close socket
    closesocket(sock);
    WSACleanup();
    
    return 0;
}