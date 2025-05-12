#pragma once
#include <string>
#include <chrono>
#include <optional>
#include <regex>

class LogEntry {
public:
    std::string user;        // Primary name
    std::string ip;          // Primary name
    std::string level;       // Primary name
    std::string message;
    std::chrono::system_clock::time_point timestamp;
    int response_time = 0;

    // Aliases to handle both naming conventions in the code
    std::string& username = user;
    std::string& ip_address = ip;
    std::string& log_level = level;
    
    // Helper methods
    static std::chrono::system_clock::time_point parse_timestamp(const std::string& timestamp_str);
    static std::optional<LogEntry> parse_log_line(const std::string& line);
};