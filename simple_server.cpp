#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <nlohmann/json.hpp>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mutex>
// Include any other necessary headers

#pragma comment(lib, "ws2_32.lib")

// Simple LogEntry structure
struct ParsedLogEntry {
    std::chrono::system_clock::time_point timestamp;
    std::string username;
    std::string ip_address;
    std::string log_level;
    std::string message;
    double response_time = 0.0;
    
    // Helper for parsing timestamps
    static std::chrono::system_clock::time_point parse_timestamp(const std::string& ts_str) {
        std::tm tm = {};
        std::istringstream ss(ts_str);
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        return std::chrono::system_clock::from_time_t(std::mktime(&tm));
    }
};

// JSON parser function
std::vector<ParsedLogEntry> parse_json_file(const std::string& filepath) {
    std::vector<ParsedLogEntry> entries;
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        std::cerr << "Failed to open JSON file: " << filepath << std::endl;
        return entries;
    }
    
    try {
        nlohmann::json j;
        file >> j;
        
        if (j.is_array()) {
            for (const auto& log : j) {
                if (!log.contains("timestamp") || !log.contains("ip_address")) {
                    continue;
                }
                
                ParsedLogEntry entry;
                entry.timestamp = ParsedLogEntry::parse_timestamp(log["timestamp"].get<std::string>());
                entry.ip_address = log["ip_address"].get<std::string>();
                
                if (log.contains("user_id")) {
                    entry.username = "user_" + std::to_string(log["user_id"].get<int>());
                } else if (log.contains("username")) {
                    entry.username = log["username"].get<std::string>();
                } else {
                    entry.username = "unknown";
                }
                
                entry.log_level = log.value("log_level", "INFO");
                entry.message = log.value("message", "");
                entry.response_time = log.value("response_time", 0.0);
                
                entries.push_back(entry);
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
    }
    
    std::cout << "Parsed " << entries.size() << " entries from JSON file" << std::endl;
    return entries;
}

// TXT parser function
std::vector<ParsedLogEntry> parse_txt_file(const std::string& filepath) {
    std::vector<ParsedLogEntry> entries;
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        std::cerr << "Failed to open TXT file: " << filepath << std::endl;
        return entries;
    }
    
    std::string line;
    int success_count = 0;
    
    while (std::getline(file, line)) {
        try {
            std::vector<std::string> parts;
            std::istringstream iss(line);
            std::string part;
            
            while (std::getline(iss, part, '|')) {
                part.erase(0, part.find_first_not_of(" \t"));
                part.erase(part.find_last_not_of(" \t") + 1);
                parts.push_back(part);
            }
            
            if (parts.size() < 5) continue;
            
            ParsedLogEntry entry;
            entry.timestamp = ParsedLogEntry::parse_timestamp(parts[0]);
            entry.log_level = parts[1];
            entry.message = parts[2];
            
            // Parse UserID
            std::string user_part = parts[3];
            size_t user_pos = user_part.find("UserID:");
            if (user_pos != std::string::npos) {
                std::string user_id_str = user_part.substr(user_pos + 7);
                user_id_str.erase(0, user_id_str.find_first_not_of(" \t"));
                entry.username = "user_" + user_id_str;
            } else {
                entry.username = "unknown";
            }
            
            // Parse IP address
            std::string ip_part = parts[4];
            size_t ip_pos = ip_part.find("IP:");
            if (ip_pos != std::string::npos) {
                entry.ip_address = ip_part.substr(ip_pos + 3);
                entry.ip_address.erase(0, entry.ip_address.find_first_not_of(" \t"));
            } else {
                entry.ip_address = "0.0.0.0";
            }
            
            entries.push_back(entry);
            success_count++;
        } catch (const std::exception& e) {
            // Skip invalid lines
        }
    }
    
    std::cout << "Parsed " << success_count << " entries from TXT file" << std::endl;
    return entries;
}

// Define a function to handle clients
void handleClient(SOCKET clientSocket) {
    char buffer[4096];
    
    // After receiving the request
    int bytesReceived = recv(clientSocket, buffer, 4096, 0);
    std::cout << "DEBUG: Bytes received from client: " << bytesReceived << std::endl;
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        std::cout << "DEBUG: Request preview: " << std::string(buffer).substr(0, 100) << "..." << std::endl;
        
        // Parse the client's request
        nlohmann::json request = nlohmann::json::parse(buffer);
        nlohmann::json response;
        
        // Extract request details
        std::string request_type = request.value("request_type", "");
        
        // Handle parse requests
        if (request_type == "parse") {
            std::string file_path = request.value("file_path", "");
            std::string file_type = request.value("file_type", "");
            
            std::cout << "Processing parse request for " << file_path << " (" << file_type << ")" << std::endl;
            
            if (file_path.empty() || file_type.empty()) {
                response["status"] = "error";
                response["message"] = "Missing file path or type";
            }
            else {
                std::vector<ParsedLogEntry> entries;
                
                // Parse based on file type
                if (file_type == "json") {
                    entries = parse_json_file(file_path);
                }
                else if (file_type == "txt") {
                    entries = parse_txt_file(file_path);
                }
                else {
                    response["status"] = "error";
                    response["message"] = "Unsupported file type: " + file_type;
                    entries.clear();
                }
                
                // Create response with parsed entries
                if (!entries.empty()) {
                    // Limit to 1000 entries max for transmission
                    const int MAX_ENTRIES_TO_SEND = 1000;
                    int entries_to_send = (entries.size() < MAX_ENTRIES_TO_SEND) ? entries.size() : MAX_ENTRIES_TO_SEND;

                    response["status"] = "success";
                    response["count"] = entries.size();  // Total count
                    response["sent_count"] = entries_to_send;  // How many we're actually sending

                    // Convert entries to JSON array
                    nlohmann::json entries_json = nlohmann::json::array();
                    for (int i = 0; i < entries_to_send; i++) {
                        const auto& entry = entries[i];
                        auto time_t_point = std::chrono::system_clock::to_time_t(entry.timestamp);
                        std::tm tm = {};
                        localtime_s(&tm, &time_t_point);
                        char time_buf[80];
                        strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", &tm);
                        
                        nlohmann::json entry_json;
                        entry_json["timestamp"] = time_buf;
                        entry_json["username"] = entry.username;
                        entry_json["ip_address"] = entry.ip_address;
                        entry_json["log_level"] = entry.log_level;
                        entry_json["message"] = entry.message;
                        entry_json["response_time"] = entry.response_time;
                        
                        entries_json.push_back(entry_json);
                    }
                    
                    response["entries"] = entries_json;
                }
                else {
                    response["status"] = "error";
                    response["message"] = "Failed to parse entries from file";
                }
            }
        }
        else {
            response["status"] = "error";
            response["message"] = "Unknown request type: " + request_type;
        }
        
        // Send the response
        std::string responseStr = response.dump();
        std::cout << "DEBUG: Sending response length: " << responseStr.size() << " bytes" << std::endl;
        std::cout << "DEBUG: Response preview: " << responseStr.substr(0, 100) << "..." << std::endl;
        send(clientSocket, responseStr.c_str(), responseStr.length(), 0);
    }
    
    // Close client socket
    closesocket(clientSocket);
}

// Main function - this was likely missing or incorrect
int main() {
    // Initialize Winsock
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);
    if (WSAStartup(ver, &wsData) != 0) {
        std::cerr << "Winsock startup failed\n";
        return 1;
    }
    
    // Create socket
    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }
    
    // Bind socket to IP and port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(8080);
    hint.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR) {
        std::cerr << "Socket bind failed\n";
        closesocket(listening);
        WSACleanup();
        return 1;
    }
    
    // Start listening
    if (listen(listening, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed\n";
        closesocket(listening);
        WSACleanup();
        return 1;
    }
    
    std::cout << "Server started on port 8080\n";
    
    // Accept connections
    while (true) {
        SOCKET clientSocket = accept(listening, nullptr, nullptr);
        if (clientSocket != INVALID_SOCKET) {
            // Create thread to handle client
            std::thread clientThread(handleClient, clientSocket);
            clientThread.detach();
        }
    }
    
    // Close socket
    closesocket(listening);
    WSACleanup();
    
    return 0;
}