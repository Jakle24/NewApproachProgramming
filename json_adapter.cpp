// json_adapter.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "src/LogEntry.hpp"

std::vector<LogEntry> parse_json_adapted(const std::string& filepath) {
    std::vector<LogEntry> entries;
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        std::cerr << "Failed to open JSON file: " << filepath << std::endl;
        return entries;
    }
    
    try {
        nlohmann::json j;
        file >> j;
        
        // Check if it's an array format (which is what we see in the logs)
        if (j.is_array()) {
            for (const auto& log : j) {
                LogEntry entry;
                
                // Required fields
                if (log.contains("timestamp") && log.contains("ip_address")) {
                    entry.timestamp = LogEntry::parse_timestamp(log["timestamp"].get<std::string>());
                    entry.ip_address = log["ip_address"].get<std::string>();
                    
                    // Handle user_id (numeric) vs username (string)
                    if (log.contains("user_id")) {
                        // Convert user_id to string
                        entry.username = "user_" + std::to_string(log["user_id"].get<int>());
                    } else if (log.contains("username")) {
                        entry.username = log["username"].get<std::string>();
                    } else {
                        entry.username = "unknown";
                    }
                    
                    // Optional fields with defaults
                    entry.log_level = log.value("log_level", "INFO");
                    entry.message = log.value("message", "");
                    entry.response_time = log.value("response_time", 0.0);
                    
                    entries.push_back(entry);
                }
            }
            
            std::cout << "Successfully parsed " << entries.size() << " logs from JSON array" << std::endl;
        }
        // If you also need to handle single-object format, add that code here
    }
    catch (const std::exception& e) {
        std::cerr << "Error parsing JSON file " << filepath << ": " << e.what() << std::endl;
    }
    
    return entries;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: json_adapter <json_file_path>\n";
        return 1;
    }
    
    std::string file_path = argv[1];
    std::vector<LogEntry> entries = parse_json_adapted(file_path);
    
    std::cout << "Parsed " << entries.size() << " entries:\n\n";
    
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