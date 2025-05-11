#pragma once
#include <string>
#include <chrono>
#include <optional>
#include <regex>

struct LogEntry {
    std::chrono::system_clock::time_point timestamp;
    std::string log_level;
    std::string username;
    std::string ip_address;
    std::string message;
    double response_time;  // in milliseconds
    
    static std::optional<LogEntry> parse_log_line(const std::string& line);
};
