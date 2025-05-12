#pragma once
#include <string>
#include <chrono>
#include <optional>

/**
 * @struct LogEntry
 * @brief Represents a single log entry with timestamp and metadata
 * 
 * This structure encapsulates all data fields found in the various log formats
 * (JSON, TXT, XML) and provides static methods for parsing log lines and timestamps.
 */
struct LogEntry {
    std::chrono::system_clock::time_point timestamp;  // When the log was created
    std::string log_level;    // Severity level (INFO, WARN, ERROR, etc.)
    std::string username;     // User associated with the log event
    std::string ip_address;   // Source IP address
    std::string message;      // Actual log message content
    double response_time;     // Performance metric in milliseconds
    
    /**
     * @brief Parses a raw log line into a structured LogEntry object
     * @param line The raw log line text to parse
     * @return Optional LogEntry object if parsing succeeded, nullopt otherwise
     */
    static std::optional<LogEntry> parse_log_line(const std::string& line);
    
    /**
     * @brief Converts a string timestamp into a system_clock time_point
     * @param timestamp_str String representation of timestamp (format: YYYY-MM-DD HH:MM:SS)
     * @return Converted time_point object
     */
    static std::chrono::system_clock::time_point parse_timestamp(const std::string& timestamp_str);
};
