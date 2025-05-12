#pragma once
#include <string>
#include <vector>
#include <map>
#include <optional>
#include <regex>
#include <filesystem>
#include "LogEntry.hpp"
#include "DateRange.hpp"

class LogProcessor {
public:
    LogProcessor(const std::string& log_directory);
    
    std::vector<LogEntry> process_logs();
    void display_statistics();
    
    // File handling
    std::vector<std::string> get_log_files(const std::string& directory);
    
    // Parsing methods - match implementation in LogProcessor.cpp
    std::vector<LogEntry> parse_txt(const std::string& filename);
    std::vector<LogEntry> parse_json(const std::string& filename);
    std::vector<LogEntry> parse_xml(const std::string& filename);
    
    // Analysis methods
    void analyze(const std::vector<LogEntry>& logs);
    void calculate_statistics(const std::vector<LogEntry>& logs);
    
    // Advanced analysis
    std::map<std::string, double> analyze_by_user(const std::vector<LogEntry>& logs, const std::optional<DateRange>& date_range = std::nullopt);
    std::map<std::string, double> analyze_by_ip(const std::vector<LogEntry>& logs, const std::optional<DateRange>& date_range = std::nullopt);
    std::map<std::string, double> analyze_by_level(const std::vector<LogEntry>& logs, const std::optional<DateRange>& date_range = std::nullopt);
    
    // Additional method for loading logs with date filtering
    std::vector<LogEntry> load_logs(const std::optional<DateRange>& date_range = std::nullopt);
    
private:
    std::string log_folder;
    std::map<std::string, int> users_count;
    std::map<std::string, int> ip_count;
    std::map<std::string, int> level_count;
};