#pragma once
#include <string>
#include <chrono>
#include <optional>

struct LogEntry {
    std::chrono::system_clock::time_point timestamp;
    std::string log_level;  // Changed from level to log_level
    std::string username;   // Changed from user to username
    std::string ip_address; // Changed from ip to ip_address
    std::string message;
    double response_time;  // in milliseconds
    
    static std::optional<LogEntry> parse_log_line(const std::string& line);
    static std::chrono::system_clock::time_point parse_timestamp(const std::string& timestamp_str);
};
