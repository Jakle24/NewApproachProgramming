#include "LogEntry.hpp"
#include <regex>
#include <sstream>
#include <iomanip>

std::optional<LogEntry> LogEntry::parse_log_line(const std::string& line) {
    // Define regex pattern for log line
    std::regex log_pattern(R"(\[(.*?)\] \[(.*?)\] \[(.*?)\] \[(.*?)\] (.+))");
    
    LogEntry entry;
    
    // Match the line against the pattern
    std::smatch matches;
    if (std::regex_match(line, matches, log_pattern)) {
        // Ensure we have the expected number of matches
        if (matches.size() != 6) {
            return std::nullopt;
        }
        
        // Parse timestamp
        entry.timestamp = parse_timestamp(matches[1].str());
        
        // Extract other fields
        entry.level = matches[2].str();    // Log level
        entry.user = matches[3].str();     // Username
        entry.ip = matches[4].str();       // IP address
        entry.message = matches[5].str();  // Message content
        
        return entry;
    }
    
    return std::nullopt;
}

std::chrono::system_clock::time_point LogEntry::parse_timestamp(const std::string& timestamp_str) {
    std::tm tm = {};
    std::istringstream ss(timestamp_str);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    
    auto time_point = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    return time_point;
}