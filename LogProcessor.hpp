#ifndef LOG_PROCESSOR_HPP
#define LOG_PROCESSOR_HPP

#include <string>
#include <vector>
#include <map>
#include <optional>
#include "LogEntry.hpp"

struct DateRange {
    std::chrono::system_clock::time_point start;
    std::chrono::system_clock::time_point end;

    bool contains(const std::chrono::system_clock::time_point& point) const {
        return point >= start && point <= end;
    }
};

class LogProcessor {
public:
    LogProcessor(const std::string& log_dir);
    void process_logs(std::optional<DateRange> range = std::nullopt);
std::vector<LogEntry> collect_logs(std::optional<DateRange> range = std::nullopt);
nlohmann::json analyze_by_user(const std::vector<LogEntry>& entries);
nlohmann::json analyze_by_ip(const std::vector<LogEntry>& entries);
nlohmann::json analyze_by_level(const std::vector<LogEntry>& entries);


private:
    std::string log_dir_;
    std::vector<std::string> get_log_files();
    std::vector<LogEntry> parse_txt(const std::string& filepath);
    std::vector<LogEntry> parse_json(const std::string& filepath);
    std::vector<LogEntry> parse_xml(const std::string& filepath);
    void analyze(const std::vector<LogEntry>& entries);
};

#endif
