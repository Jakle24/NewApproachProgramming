#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <algorithm>

#pragma comment(lib, "ws2_32.lib")

void printUsage() {
    std::cout << "Usage:\n";
    std::cout << "  For analysis: simple_client --analysis --log-folder <folder> --type <user|ip|level>\n";
    std::cout << "  For parsing:  simple_client --parse --file <path> --type <json|txt>\n";
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printUsage();
        return 1;
    }
    
    std::string mode = argv[1];
    nlohmann::json request;
    
    if (mode == "--analysis") {
        // Original analysis mode
        if (argc < 6) {
            printUsage();
            return 1;
        }
        
        std::string log_folder;
        std::string analysis_type;
        
        for (int i = 2; i < argc; i += 2) {
            std::string arg = argv[i];
            if (arg == "--log-folder" && i + 1 < argc) {
                log_folder = argv[i + 1];
            } 
            else if (arg == "--type" && i + 1 < argc) {
                analysis_type = argv[i + 1];
            }
        }
        
        if (log_folder.empty() || analysis_type.empty()) {
            std::cout << "Missing required parameters\n";
            printUsage();
            return 1;
        }
        
        request["request_type"] = "analysis";
        request["log_folder"] = log_folder;
        request["analysis_type"] = analysis_type;
    }
    else if (mode == "--parse") {
        // New parsing mode
        if (argc < 6) {
            printUsage();
            return 1;
        }
        
        std::string file_path;
        std::string file_type;
        
        for (int i = 2; i < argc; i += 2) {
            std::string arg = argv[i];
            if (arg == "--file" && i + 1 < argc) {
                file_path = argv[i + 1];
            } 
            else if (arg == "--type" && i + 1 < argc) {
                file_type = argv[i + 1];
            }
        }
        
        if (file_path.empty() || file_type.empty()) {
            std::cout << "Missing required parameters\n";
            printUsage();
            return 1;
        }
        
        request["request_type"] = "parse";
        request["file_path"] = file_path;
        request["file_type"] = file_type;
    }
    else {
        std::cout << "Unknown mode: " << mode << "\n";
        printUsage();
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
    
    // Send request
    std::string requestStr = request.dump();
    std::cout << "DEBUG: Sending request: " << requestStr << std::endl;
    std::cout << "DEBUG: Request length: " << requestStr.size() << " bytes" << std::endl;
    send(sock, requestStr.c_str(), requestStr.size(), 0);
    
    // Receive response
    char buffer[65536]; // Larger buffer for potentially large responses
    int bytesReceived = recv(sock, buffer, 65536, 0);
    std::cout << "DEBUG: Bytes received: " << bytesReceived << std::endl;
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        std::cout << "DEBUG: Response preview: " << std::string(buffer).substr(0, 100) << "..." << std::endl;
        
        // Parse and display results
        nlohmann::json response = nlohmann::json::parse(buffer);
        
        if (mode == "--analysis") {
            // Display analysis results
            std::cout << "\n=== Analysis Results ===\n\n";
            std::cout << response.dump(4) << std::endl;  // Pretty-print with 4-space indent
        }
        else if (mode == "--parse") {
            // Display parsing results
            if (response.contains("status") && response["status"] == "success") {
                std::cout << "\n=== Parsing Results ===\n";
                std::cout << "Successfully parsed " << response["count"].get<int>() << " entries\n\n";
                
                // Display first few entries
                int entries_size = static_cast<int>(response["entries"].size());
                int display_count = (entries_size < 5) ? entries_size : 5;
                for (int i = 0; i < display_count; i++) {
                    const auto& entry = response["entries"][i];
                    
                    std::cout << "------------------------------------\n";
                    std::cout << "Timestamp: " << entry["timestamp"].get<std::string>() << "\n";
                    std::cout << "Username: " << entry["username"].get<std::string>() << "\n";
                    std::cout << "IP Address: " << entry["ip_address"].get<std::string>() << "\n";
                    std::cout << "Log Level: " << entry["log_level"].get<std::string>() << "\n";
                    std::cout << "Message: " << entry["message"].get<std::string>() << "\n";
                }
                
                if (display_count < (int)response["entries"].size()) {
                    std::cout << "\n... and " << (response["entries"].size() - display_count) 
                              << " more entries\n";
                }
            }
            else {
                std::cout << "Error: " << response["message"].get<std::string>() << std::endl;
            }
        }
    }
    
    // Close socket
    closesocket(sock);
    WSACleanup();
    
    return 0;
}