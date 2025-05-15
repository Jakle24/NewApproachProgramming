// simple_server.cpp - Multithreaded log parsing server
// Handles client requests to parse different log file formats (JSON, TXT)
// Key components:
// - ParsedLogEntry struct: Holds structured log data
// - parse_json_file/parse_txt_file: Format-specific parsers
// - handleClient: Connection handler for client requests
// - main: Socket setup and request dispatching

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
#include <map>
#include <filesystem>
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

/*
// XML parser function
// Note: This implementation is commented out as it requires the tinyxml2 library
// Uncomment the following code if you want to enable XML parsing (TESTING ONLY, NOT CONSISTENT)
std::vector<ParsedLogEntry> parse_xml_file(const std::string& filepath) {
    std::vector<ParsedLogEntry> entries;
    
    // Load XML file
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError result = doc.LoadFile(filepath.c_str());
    
    if (result != tinyxml2::XML_SUCCESS) {
        std::cerr << "Failed to load XML file: " << filepath << std::endl;
        return entries;
    }
    
    // Get root element
    tinyxml2::XMLElement* root = doc.RootElement();
    if (!root) {
        std::cerr << "XML file has no root element" << std::endl;
        return entries;
    }
    
    // Check if root is "logs"
    if (std::string(root->Name()) != "logs") {
        std::cerr << "XML root element is not 'logs'" << std::endl;
        return entries;
    }
    
    // Iterate through log entries
    int success_count = 0;
    for (tinyxml2::XMLElement* logElement = root->FirstChildElement("log");
         logElement;
         logElement = logElement->NextSiblingElement("log")) {
        
        try {
            ParsedLogEntry entry;
            
            // Parse timestamp
            tinyxml2::XMLElement* timestampElement = logElement->FirstChildElement("timestamp");
            if (timestampElement && timestampElement->GetText()) {
                entry.timestamp = ParsedLogEntry::parse_timestamp(timestampElement->GetText());
            } else {
                // Default timestamp if not found
                entry.timestamp = std::chrono::system_clock::now();
            }
            
            // Parse username (might be user_id in some formats)
            tinyxml2::XMLElement* userElement = logElement->FirstChildElement("username");
            if (userElement && userElement->GetText()) {
                entry.username = userElement->GetText();
            } else {
                // Try user_id instead
                userElement = logElement->FirstChildElement("user_id");
                if (userElement && userElement->GetText()) {
                    entry.username = "user_" + std::string(userElement->GetText());
                } else {
                    entry.username = "unknown";
                }
            }
            
            // Parse IP address
            tinyxml2::XMLElement* ipElement = logElement->FirstChildElement("ip_address");
            if (ipElement && ipElement->GetText()) {
                entry.ip_address = ipElement->GetText();
            } else {
                entry.ip_address = "0.0.0.0";
            }
            
            // Parse log level
            tinyxml2::XMLElement* levelElement = logElement->FirstChildElement("log_level");
            if (levelElement && levelElement->GetText()) {
                entry.log_level = levelElement->GetText();
            } else {
                entry.log_level = "INFO";
            }
            
            // Parse message
            tinyxml2::XMLElement* messageElement = logElement->FirstChildElement("message");
            if (messageElement && messageElement->GetText()) {
                entry.message = messageElement->GetText();
            } else {
                entry.message = "";
            }
            
            // Parse response time (optional)
            tinyxml2::XMLElement* rtElement = logElement->FirstChildElement("response_time");
            if (rtElement && rtElement->GetText()) {
                try {
                    entry.response_time = std::stod(rtElement->GetText());
                } catch (const std::exception&) {
                    entry.response_time = 0.0;
                }
            }
            
            entries.push_back(entry);
            success_count++;
            
        } catch (const std::exception& e) {
            std::cerr << "Error parsing XML log entry: " << e.what() << std::endl;
        }
    }
    
    std::cout << "Parsed " << success_count << " entries from XML file" << std::endl;
    return entries;
}
*/

// Analyze logs by IP address
nlohmann::json analyze_by_ip(const std::string& log_folder) {
    nlohmann::json result;
    std::map<std::string, int> ip_counts;
    
    // Process log files in the folder
    for (const auto& entry : std::filesystem::directory_iterator(log_folder)) {
        std::string file_path = entry.path().string();
        std::string extension = entry.path().extension().string();
        
        std::vector<ParsedLogEntry> logs;
        if (extension == ".json") {
            logs = parse_json_file(file_path);
        } 
        else if (extension == ".txt") {
            logs = parse_txt_file(file_path);
        }
        
        // Count occurrences of each IP
        for (const auto& log : logs) {
            ip_counts[log.ip_address]++;
        }
    }
    
    // Create analysis result
    result["status"] = "success";
    result["analysis_type"] = "ip";
    
    nlohmann::json ip_stats = nlohmann::json::array();
    for (const auto& [ip, count] : ip_counts) {
        ip_stats.push_back({
            {"ip_address", ip},
            {"count", count}
        });
    }
    
    result["ip_statistics"] = ip_stats;
    return result;
}

// Analyze logs by user activity
nlohmann::json analyze_by_user(const std::string& log_folder) {
    nlohmann::json result;
    std::map<std::string, int> user_counts;
    
    // Process log files in the folder
    for (const auto& entry : std::filesystem::directory_iterator(log_folder)) {
        std::string file_path = entry.path().string();
        std::string extension = entry.path().extension().string();
        
        std::vector<ParsedLogEntry> logs;
        if (extension == ".json") {
            logs = parse_json_file(file_path);
        } 
        else if (extension == ".txt") {
            logs = parse_txt_file(file_path);
        }
        
        // Count occurrences of each username
        for (const auto& log : logs) {
            user_counts[log.username]++;
        }
    }
    
    // Create analysis result
    result["status"] = "success";
    result["analysis_type"] = "user";
    
    nlohmann::json user_stats = nlohmann::json::array();
    for (const auto& [username, count] : user_counts) {
        user_stats.push_back({
            {"username", username},
            {"count", count}
        });
    }
    
    result["user_statistics"] = user_stats;
    return result;
}

// Analyze logs by severity level
nlohmann::json analyze_by_level(const std::string& log_folder) {
    nlohmann::json result;
    std::map<std::string, int> level_counts;
    
    // Process log files in the folder
    for (const auto& entry : std::filesystem::directory_iterator(log_folder)) {
        std::string file_path = entry.path().string();
        std::string extension = entry.path().extension().string();
        
        std::vector<ParsedLogEntry> logs;
        if (extension == ".json") {
            logs = parse_json_file(file_path);
        } 
        else if (extension == ".txt") {
            logs = parse_txt_file(file_path);
        }
        
        // Count occurrences of each log level
        for (const auto& log : logs) {
            level_counts[log.log_level]++;
        }
    }
    
    // Create analysis result
    result["status"] = "success";
    result["analysis_type"] = "level";
    
    nlohmann::json level_stats = nlohmann::json::array();
    for (const auto& [level, count] : level_counts) {
        level_stats.push_back({
            {"level", level},
            {"count", count}
        });
    }
    
    result["level_statistics"] = level_stats;
    return result;
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
        
        // Handle different request types
        if (request_type == "parse") {
            std::string file_path = request.value("file_path", "");
            std::string file_type = request.value("file_type", "");
            
            std::vector<ParsedLogEntry> logs;
            
            if (file_type == "json") {
                logs = parse_json_file(file_path);
            } 
            else if (file_type == "txt") {
                logs = parse_txt_file(file_path);
            }
            /*
            // Uncomment to enable XML parsing
            else if (file_type == "xml") {
                logs = parse_xml_file(file_path);
            }
            */
            else {
                response["status"] = "error";
                response["message"] = "Unsupported file type: " + file_type;
            }
            
            // Create response with parsed entries
            if (!logs.empty()) {
                // Limit to 1000 entries max for transmission
                const int MAX_ENTRIES_TO_SEND = 1000;
                int entries_to_send = (logs.size() < MAX_ENTRIES_TO_SEND) ? logs.size() : MAX_ENTRIES_TO_SEND;

                response["status"] = "success";
                response["count"] = logs.size();  // Total count
                response["sent_count"] = entries_to_send;  // How many we're actually sending

                // Convert entries to JSON array
                nlohmann::json entries_json = nlohmann::json::array();
                for (int i = 0; i < entries_to_send; i++) {
                    const auto& entry = logs[i];
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
        else if (request_type == "analysis") {
            std::string log_folder = request.value("log_folder", "");
            std::string analysis_type = request.value("analysis_type", "");
            
            std::cout << "Processing analysis request for " << log_folder << " (" << analysis_type << ")" << std::endl;
            
            if (log_folder.empty() || analysis_type.empty()) {
                response["status"] = "error";
                response["message"] = "Missing log_folder or analysis_type";
            }
            else {
                // Perform requested analysis
                if (analysis_type == "ip") {
                    response = analyze_by_ip(log_folder);
                }
                else if (analysis_type == "user") {
                    response = analyze_by_user(log_folder);
                }
                else if (analysis_type == "level") {
                    response = analyze_by_level(log_folder);
                }
                else {
                    response["status"] = "error";
                    response["message"] = "Unsupported analysis type: " + analysis_type;
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

// Main function
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