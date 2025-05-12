#include "LogEntry.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <regex>

std::chrono::system_clock::time_point LogEntry::parse_timestamp(const std::string& timestamp_str) {
    std::tm tm = {};
    std::istringstream ss(timestamp_str);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) {
        return std::chrono::system_clock::now(); // Return current time if parsing fails
    }
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

std::optional<LogEntry> LogEntry::parse_log_line(const std::string& line) {
    try {
        // This regex pattern matches common log formats
        // Adjust as needed for your specific log format
        std::regex log_pattern(
            R"((\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})\s+(\w+)\s+\[(\w+)\]\s+\[(\S+)\]\s+(.+?)(?:\s+\[(\d+(?:\.\d+)?)ms\])?$)"
        );
        
        std::smatch matches;
        if (!std::regex_match(line, matches, log_pattern)) {
            return std::nullopt;
        }
        
        LogEntry entry;
        
        // Parse timestamp
        entry.timestamp = parse_timestamp(matches[1].str());
        
        // Parse other fields - using the correct field names
        entry.log_level = matches[2].str();
        entry.username = matches[3].str();
        entry.ip_address = matches[4].str();
        entry.message = matches[5].str();
        
        // Parse response time if available
        if (matches[6].matched) {
            entry.response_time = std::stod(matches[6].str());
        } else {
            entry.response_time = 0.0;
        }
        
        return entry;
    }
    catch (const std::exception& e) {
        std::cerr << "Error parsing log line: " << e.what() << std::endl;
        return std::nullopt;
    }
}
