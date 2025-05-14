#pragma once
#include <string>
#include <vector>
#include <optional>
#include <chrono>
#include <nlohmann::json.hpp>
#include <thread>
#include <mutex>
#include "LogEntry.hpp"

/**
 * @struct DateRange
 * @brief Defines a time period for filtering log entries
 * 
 * Used to restrict log analysis to entries within a specific timeframe.
 */
struct DateRange {
    std::chrono::system_clock::time_point start;  // Inclusive start time
    std::chrono::system_clock::time_point end;    // Inclusive end time
};

/**
 * @class LogProcessor
 * @brief Core component for parsing and analyzing log files
 * 
 * Responsible for loading logs from various formats, applying filters,
 * and performing statistical analysis on the log data.
 */
class LogProcessor {
public:
    /**
     * @brief Constructs a LogProcessor that processes logs from the specified folder
     * @param log_folder Directory path containing log files to analyze
     */
    explicit LogProcessor(const std::string& folder)
      : log_folder(folder) 
    {}

    /**
     * @brief Analyzes logs grouped by username
     * @param date_range Optional time range to filter logs
     * @return JSON object containing user-based statistics
     */
    nlohmann::json analyze_by_user(const std::optional<DateRange>& date_range = std::nullopt);
    
    /**
     * @brief Analyzes logs grouped by IP address
     * @param date_range Optional time range to filter logs
     * @return JSON object containing IP-based statistics
     */
    nlohmann::json analyze_by_ip(const std::optional<DateRange>& date_range = std::nullopt);
    
    /**
     * @brief Analyzes logs grouped by severity level
     * @param date_range Optional time range to filter logs
     * @return JSON object containing log level statistics
     */
    nlohmann::json analyze_by_level(const std::optional<DateRange>& date_range = std::nullopt);
    
    /**
     * @brief Retrieves a list of log files in the configured folder
     * @return Vector of file paths to process
     */
    std::vector<std::string> get_log_files();
    
    /**
     * @brief Parses plain text log files
     * @param file_path Path to the log file
     * @return Vector of parsed LogEntry objects
     */
    std::vector<LogEntry> parse_txt(const std::string& file_path);
    
    /**
     * @brief Parses JSON format log files
     * @param file_path Path to the log file
     * @return Vector of parsed LogEntry objects
     */
    std::vector<LogEntry> parse_json(const std::string& file_path);
    
    /**
     * @brief Parses XML format log files
     * @param file_path Path to the log file
     * @return Vector of parsed LogEntry objects
     */
    std::vector<LogEntry> parse_xml(const std::string& file_path);
    
    /**
     * @brief Generic analysis function
     * @param logs Collection of log entries to analyze
     */
    void analyze(const std::vector<LogEntry>& logs);
    
    /**
     * @brief Processes logs in parallel
     * @param date_range Optional time range to filter logs
     * @return Vector of processed LogEntry objects
     */
    std::vector<LogEntry> process_logs_parallel(const std::optional<DateRange>& date_range);

private:
    std::string log_folder;  // Directory containing log files to process
    
    /**
     * @brief Loads and filters logs from all supported file formats
     * @param date_range Optional filter criteria for log timestamps
     * @return Combined vector of filtered log entries
     */
    std::vector<LogEntry> load_logs(const std::optional<DateRange>& date_range);
    
    /**
     * @brief Calculates statistical metrics for a set of numeric values
     * @param values Collection of numeric data points
     * @return JSON object containing min, max, average, median, and count
     */
    nlohmann::json calculate_statistics(const std::vector<double>& values);

    /**
     * @brief Extracts the content of a specific XML tag
     * @param xml XML string to parse
     * @param tag Name of the tag to extract
     * @return Content of the specified XML tag
     */
    std::string extract_xml_tag(const std::string& xml, const std::string& tag);
};
