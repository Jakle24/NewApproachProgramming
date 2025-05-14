#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <regex>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;

// Simple LogEntry structure
struct LogEntry {
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

// Direct JSON parser function
std::vector<LogEntry> parse_json_file(const std::string& filepath) {
    std::vector<LogEntry> entries;
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        std::cerr << "Failed to open JSON file: " << filepath << std::endl;
        return entries;
    }
    
    try {
        nlohmann::json j;
        file >> j;
        
        std::cout << "JSON structure type: " << (j.is_array() ? "array" : "object") << std::endl;
        if (!j.is_array()) {
            std::cerr << "Expected JSON array format" << std::endl;
            return entries;
        }
        
        // Process each log entry in the array
        for (const auto& log : j) {
            if (!log.contains("timestamp") || !log.contains("ip_address")) {
                std::cout << "Skipping entry - missing required fields" << std::endl;
                continue;
            }
            
            LogEntry entry;
            
            // Parse timestamp
            entry.timestamp = LogEntry::parse_timestamp(log["timestamp"].get<std::string>());
            
            // Get IP address
            entry.ip_address = log["ip_address"].get<std::string>();
            
            // Handle user_id vs username
            if (log.contains("user_id")) {
                entry.username = "user_" + std::to_string(log["user_id"].get<int>());
            } else if (log.contains("username")) {
                entry.username = log["username"].get<std::string>();
            } else {
                entry.username = "unknown";
            }
            
            // Get other fields
            entry.log_level = log.value("log_level", "INFO");
            entry.message = log.value("message", "");
            entry.response_time = log.value("response_time", 0.0);
            
            entries.push_back(entry);
        }
        
        std::cout << "Successfully parsed " << entries.size() << " entries from JSON array" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
    }
    
    return entries;
}

// TXT parser function for the format:
// TIMESTAMP | LOG_LEVEL | MESSAGE | UserID: USER_ID | IP: IP_ADDRESS
std::vector<LogEntry> parse_txt_file(const std::string& filepath) {
    std::vector<LogEntry> entries;
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        std::cerr << "Failed to open TXT file: " << filepath << std::endl;
        return entries;
    }
    
    std::string line;
    int line_count = 0;
    int success_count = 0;
    
    while (std::getline(file, line)) {
        line_count++;
        try {
            // Split by pipe delimiter
            std::vector<std::string> parts;
            std::istringstream iss(line);
            std::string part;
            
            // First extract the parts by splitting on |
            while (std::getline(iss, part, '|')) {
                // Trim whitespace
                part.erase(0, part.find_first_not_of(" \t"));
                part.erase(part.find_last_not_of(" \t") + 1);
                parts.push_back(part);
            }
            
            // Need at least 5 parts: timestamp, log_level, message, user_id, ip
            if (parts.size() < 5) {
                std::cerr << "Line " << line_count << ": Invalid format, insufficient parts" << std::endl;
                continue;
            }
            
            LogEntry entry;
            
            // Parse timestamp (first part)
            entry.timestamp = LogEntry::parse_timestamp(parts[0]);
            
            // Parse log level (second part)
            entry.log_level = parts[1];
            
            // Parse message (third part)
            entry.message = parts[2];
            
            // Parse UserID (fourth part)
            std::string user_part = parts[3];
            size_t user_pos = user_part.find("UserID:");
            if (user_pos != std::string::npos) {
                std::string user_id_str = user_part.substr(user_pos + 7); // "UserID:" is 7 chars
                user_id_str.erase(0, user_id_str.find_first_not_of(" \t"));
                entry.username = "user_" + user_id_str;
            } else {
                entry.username = "unknown";
            }
            
            // Parse IP address (fifth part)
            std::string ip_part = parts[4];
            size_t ip_pos = ip_part.find("IP:");
            if (ip_pos != std::string::npos) {
                entry.ip_address = ip_part.substr(ip_pos + 3); // "IP:" is 3 chars
                entry.ip_address.erase(0, entry.ip_address.find_first_not_of(" \t"));
            } else {
                entry.ip_address = "0.0.0.0";
            }
            
            entries.push_back(entry);
            success_count++;
            
        } catch (const std::exception& e) {
            std::cerr << "Error parsing line " << line_count << ": " << e.what() << std::endl;
        }
    }
    
    std::cout << "Successfully parsed " << success_count << " entries from TXT file (out of " 
              << line_count << " lines)" << std::endl;
    
    return entries;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: test_parse <file_path> <file_type>\n";
        std::cout << "  file_type can be: json, txt\n";
        return 1;
    }

    std::string file_path = argv[1];
    std::string file_type = argv[2];
    std::vector<LogEntry> entries;
    
    if (file_type == "json") {
        entries = parse_json_file(file_path);
    } else if (file_type == "txt") {
        entries = parse_txt_file(file_path);
    } else {
        std::cout << "Unsupported file type: " << file_type << "\n";
        std::cout << "Supported types: json, txt\n";
        return 1;
    }
    
    // Display parsed entries
    std::cout << "\n--- Parsed " << entries.size() << " entries ---\n\n";
    
    for (const auto& entry : entries) {
        auto time_t_point = std::chrono::system_clock::to_time_t(entry.timestamp);
        std::tm tm = {};
        localtime_s(&tm, &time_t_point);
        
        std::cout << "------------------------------------\n";
        std::cout << "Timestamp: " << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "\n";
        std::cout << "Username: " << entry.username << "\n";
        std::cout << "IP Address: " << entry.ip_address << "\n";
        std::cout << "Log Level: " << entry.log_level << "\n";
        std::cout << "Message: " << entry.message << "\n";
        std::cout << "Response Time: " << entry.response_time << " ms\n";
    }
    
    return 0;
}