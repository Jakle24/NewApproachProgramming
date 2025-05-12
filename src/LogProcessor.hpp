#pragma once
#include <string>
#include <vector>
#include <optional>
#include <chrono>
#include <nlohmann/json.hpp>
#include "LogEntry.hpp"

struct DateRange {
    std::chrono::system_clock::time_point start;
    std::chrono::system_clock::time_point end;
};

class LogProcessor {
public:
    LogProcessor(const std::string& log_folder);
    
    // Process logs and return statistics as JSON
    nlohmann::json analyze_by_user(const std::optional<DateRange>& date_range = std::nullopt);
    nlohmann::json analyze_by_ip(const std::optional<DateRange>& date_range = std::nullopt);
    nlohmann::json analyze_by_level(const std::optional<DateRange>& date_range = std::nullopt);
    
    // Helper methods for parsing different file formats
    std::vector<std::string> get_log_files();
    std::vector<LogEntry> parse_txt(const std::string& file_path);
    std::vector<LogEntry> parse_json(const std::string& file_path);
    std::vector<LogEntry> parse_xml(const std::string& file_path);
    
    // The analyze method that was causing issues - fix to match implementation exactly
    void analyze(const std::vector<LogEntry>& logs);
    
private:
    std::string log_folder;
    std::vector<LogEntry> load_logs(const std::optional<DateRange>& date_range);
    nlohmann::json calculate_statistics(const std::vector<double>& values);
};
