#pragma once
#include <string>
#include <vector>
#include <optional>
#include <chrono>
#include <nlohmann/json.hpp>  // Correct include path
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
    
private:
    std::string log_folder;
    std::vector<LogEntry> load_logs(const std::optional<DateRange>& date_range);
    // Helper methods for calculations
    nlohmann::json calculate_statistics(const std::vector<double>& values);
};
