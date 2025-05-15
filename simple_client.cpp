// simple_client.cpp - Log analysis client application
// Sends parsing requests to server and displays results
// Key components:
// - Command line arg processing for different modes
// - Socket communication with parsing server
// - JSON request formatting
// - Response handling and display formatting

#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <algorithm>
#include <map>

#pragma comment(lib, "ws2_32.lib")

void printUsage() {
    std::cout << "Usage:\n";
    std::cout << "  For analysis: simple_client --analysis --log-folder <folder> --type <user|ip|level>\n";
    std::cout << "  For parsing:  simple_client --parse --file <path> --type <json|txt|xml>\n";
}

// Add to analyze log levels
void analyze_log_levels(const nlohmann::json& response) {
    std::map<std::string, int> level_counts;
    
    for (const auto& entry : response["entries"]) {
        std::string level = entry["log_level"];
        level_counts[level]++;
    }
    
    std::cout << "\n=== Log Level Analysis ===\n";
    for (const auto& [level, count] : level_counts) {
        std::cout << level << ": " << count << " entries\n";
    }
}

// Add to analyze timestamps
void analyze_timestamps(const nlohmann::json& response) {
    std::map<std::string, int> date_counts;
    
    for (const auto& entry : response["entries"]) {
        std::string timestamp = entry["timestamp"].get<std::string>();
        std::string date = timestamp.substr(0, 10); // Get just the date part
        date_counts[date]++;
    }
    
    std::cout << "\n=== Date Distribution Analysis ===\n";
    for (const auto& [date, count] : date_counts) {
        std::cout << date << ": " << count << " entries\n";
    }
}

// Add to analyze top active users
void analyze_users(const nlohmann::json& response) {
    // Check if we have the expected fields
    if (!response.contains("user_statistics")) {
        std::cout << "Error: Response doesn't contain user statistics data\n";
        return;
    }
    
    // If user_statistics is empty, report it
    if (response["user_statistics"].empty()) {
        std::cout << "\n=== No User Data Available ===\n";
        std::cout << "The server returned no user statistics. This might be because:\n";
        std::cout << "- No log files were found\n";
        std::cout << "- The logs didn't contain user information\n";
        std::cout << "- There was an error processing the logs\n";
        return;
    }
    
    // Process user statistics as returned by server
    std::cout << "\n=== Top Active Users ===\n";
    
    // Create vector of pairs for sorting
    std::vector<std::pair<std::string, int>> user_pairs;
    
    for (const auto& user_stat : response["user_statistics"]) {
        if (user_stat.contains("username") && user_stat.contains("count")) {
            std::string username = user_stat["username"];
            int count = user_stat["count"];
            user_pairs.push_back({username, count});
        }
    }
    
    // Sort by count (descending)
    std::sort(user_pairs.begin(), user_pairs.end(), 
              [](const auto& a, const auto& b) { 
                  return a.second > b.second; 
              });
    
    int displayed = 0;
    for (const auto& [user, entries] : user_pairs) {
        std::cout << user << ": " << entries << " entries\n";
        displayed++;
        if (displayed >= 5) break; // Show top 5
    }
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
    std::string total_response;
    char buffer[4096];
    int bytesReceived;

    // Keep receiving until no more data
    do {
        bytesReceived = recv(sock, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            total_response.append(buffer, bytesReceived);
        }
    } while (bytesReceived > 0);

    std::cout << "DEBUG: Total bytes received: " << total_response.size() << std::endl;

    // Then parse the complete response
    nlohmann::json response = nlohmann::json::parse(total_response);
    
    if (mode == "--analysis") {
        // Display analysis results
        std::cout << "\n=== Analysis Results ===\n\n";
        std::cout << response.dump(4) << std::endl;  // Pretty-print with 4-space indent

        // Analyze log levels
        analyze_log_levels(response);

        // Analyze timestamps
        analyze_timestamps(response);

        // Analyze top active users
        analyze_users(response);
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
    
    // Close socket
    closesocket(sock);
    WSACleanup();
    
    return 0;
}