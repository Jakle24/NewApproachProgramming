#include "LogProcessor.hpp"
#include "LogEntry.hpp"
#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>


namespace fs = std::filesystem;
using json = nlohmann::json;

// Constructor
LogProcessor::LogProcessor(const std::string& log_dir)
    : log_dir_(log_dir) {}

// Helper: List files in the log directory
std::vector<std::string> LogProcessor::get_log_files() {
    std::vector<std::string> files;
    for (const auto& entry : fs::directory_iterator(log_dir_)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path().string());
        }
    }
    return files;
}

// Helper: Parse TXT/CSV logs
std::vector<LogEntry> LogProcessor::parse_txt(const std::string& filepath) {
    std::vector<LogEntry> entries;
    std::ifstream file(filepath);
    std::string line;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string user, ip, level, timestamp;

        std::getline(ss, user, ',');
        std::getline(ss, ip, ',');
        std::getline(ss, level, ',');
        std::getline(ss, timestamp, ',');

        LogEntry entry{user, ip, level, timestamp};
        if (entry.parse_timestamp()) {
            entries.push_back(entry);
        }
    }
    return entries;
}

using json = nlohmann::json;

std::vector<LogEntry> LogProcessor::parse_json(const std::string& filepath) {
    std::vector<LogEntry> entries;
    std::ifstream file(filepath);
    json j;

    try {
        file >> j;

        // Optional: check the structure
        if (!j.contains("logs") || !j["logs"].is_array()) {
            std::cerr << "Invalid JSON structure in: " << filepath << "\n";
            return entries;
        }

        for (const auto& log : j["logs"]) {
            if (log.contains("user") && log.contains("ip") && log.contains("level") && log.contains("timestamp")) {
                LogEntry entry;
                entry.user = log["user"].get<std::string>();
                entry.ip = log["ip"].get<std::string>();
                entry.level = log["level"].get<std::string>();
                entry.timestamp = log["timestamp"].get<std::string>();

                if (entry.parse_timestamp()) {
                    entries.push_back(entry);
                }
            } else {
                std::cerr << "Skipping incomplete log entry.\n";
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON file " << filepath << ": " << e.what() << "\n";
    }

    return entries;
}


// TODO: XML parser placeholder
std::vector<LogEntry> LogProcessor::parse_xml(const std::string& filepath) {
    std::cerr << "XML parsing not implemented yet: " << filepath << std::endl;
    return {};
}

// Analyze entries (by user, IP, level)
void LogProcessor::analyze(const std::vector<LogEntry>& entries) {
    std::map<std::string, int> by_user;
    std::map<std::string, int> by_ip;
    std::map<std::string, int> by_level;

    for (const auto& e : entries) {
        by_user[e.user]++;
        by_ip[e.ip]++;
        by_level[e.level]++;
    }

    std::cout << "\n--- Log Attempts by User ---\n";
    for (const auto& [user, count] : by_user) {
        std::cout << user << ": " << count << "\n";
    }

    std::cout << "\n--- Log Attempts by IP ---\n";
    for (const auto& [ip, count] : by_ip) {
        std::cout << ip << ": " << count << "\n";
    }

    std::cout << "\n--- Log Attempts by Level ---\n";
    for (const auto& [level, count] : by_level) {
        std::cout << level << ": " << count << "\n";
    }
}

// Main processing method
void LogProcessor::process_logs(std::optional<DateRange> range) {
    std::vector<LogEntry> all_entries;

    auto files = get_log_files();
    for (const auto& file : files) {
        std::vector<LogEntry> entries;

        if (file.ends_with(".json")) {
            entries = parse_json(file);
        } else if (file.ends_with(".xml")) {
            entries = parse_xml(file);
        } else {
            entries = parse_txt(file);
        }

        for (auto& e : entries) {
            if (!range || range->contains(e.parsed_time)) {
                all_entries.push_back(e);
            }
        }
    }

    if (all_entries.empty()) {
        std::cout << "No valid log entries found.\n";
    } else {
        analyze(all_entries);
    }
}

std::vector<LogEntry> LogProcessor::collect_logs(std::optional<DateRange> range) {
    std::vector<LogEntry> all_entries;

    auto files = get_log_files();
    for (const auto& file : files) {
        std::vector<LogEntry> entries;

        if (file.ends_with(".json")) {
            entries = parse_json(file);
        } else if (file.ends_with(".xml")) {
            entries = parse_xml(file);  // You can stub this for now
        } else {
            entries = parse_txt(file);
        }

        for (auto& e : entries) {
            if (!range || range->contains(e.parsed_time)) {
                all_entries.push_back(e);
            }
        }
    }

    return all_entries;
}

nlohmann::json LogProcessor::analyze_by_user(const std::vector<LogEntry>& entries) {
    std::map<std::string, int> counts;
    for (const auto& e : entries) {
        counts[e.user]++;
    }

    nlohmann::json result;
    for (const auto& [user, count] : counts) {
        result[user] = count;
    }
    return result;
}

nlohmann::json LogProcessor::analyze_by_ip(const std::vector<LogEntry>& entries) {
    std::map<std::string, int> counts;
    for (const auto& e : entries) {
        counts[e.ip]++;
    }

    nlohmann::json result;
    for (const auto& [ip, count] : counts) {
        result[ip] = count;
    }
    return result;
}

nlohmann::json LogProcessor::analyze_by_level(const std::vector<LogEntry>& entries) {
    std::map<std::string, int> counts;
    for (const auto& e : entries) {
        counts[e.level]++;
    }

    nlohmann::json result;
    for (const auto& [level, count] : counts) {
        result[level] = count;
    }
    return result;
}
