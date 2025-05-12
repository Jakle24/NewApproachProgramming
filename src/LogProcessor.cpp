// filepath: src\LogProcessor.cpp
#include "LogProcessor.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <filesystem>
#include <algorithm>
#include <numeric>  // For std::accumulate
#include <nlohmann/json.hpp>  // For JSON support

namespace fs = std::filesystem;

LogProcessor::LogProcessor(const std::string& log_directory) : log_folder(log_directory) {
    // Initialize maps
    users_count = {};
    ip_count = {};
    level_count = {};
}

std::vector<std::string> LogProcessor::get_log_files(const std::string& directory) {
    std::vector<std::string> files;
    
    for (const auto& entry : fs::directory_iterator(directory)) {
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

std::vector<LogEntry> LogProcessor::load_logs(const std::optional<DateRange>& date_range) {
    std::vector<LogEntry> logs;
    
    try {
        for (const auto& entry : fs::directory_iterator(log_folder)) {
            if (entry.path().extension() == ".log") {
                std::ifstream file(entry.path());
                if (!file.is_open()) {
                    std::cerr << "Failed to open: " << entry.path() << std::endl;
                    continue;
                }
                
                std::string line;
                while (std::getline(file, line)) {
                    auto log_entry = LogEntry::parse_log_line(line);
                    if (log_entry) {
                        // Apply date filter if specified
                        if (date_range) {
                            if (log_entry->timestamp >= date_range->start && 
                                log_entry->timestamp <= date_range->end) {
                                logs.push_back(*log_entry);
                            }
                        } else {
                            logs.push_back(*log_entry);
                        }
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading logs: " << e.what() << std::endl;
    }
    
    return logs;
}

nlohmann::json LogProcessor::calculate_statistics(const std::vector<double>& values) {
    nlohmann::json stats;
    
    if (values.empty()) {
        stats["count"] = 0;
        stats["min"] = 0;
        stats["max"] = 0;
        stats["average"] = 0;
        stats["median"] = 0;
        return stats;
    }
    
    // Calculate statistics
    stats["count"] = values.size();
    stats["min"] = *std::min_element(values.begin(), values.end());
    stats["max"] = *std::max_element(values.begin(), values.end());
    
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    stats["average"] = sum / values.size();
    
    // Calculate median
    std::vector<double> sorted_values = values;
    std::sort(sorted_values.begin(), sorted_values.end());
    size_t middle = sorted_values.size() / 2;
    if (sorted_values.size() % 2 == 0) {
        stats["median"] = (sorted_values[middle-1] + sorted_values[middle]) / 2.0;
    } else {
        stats["median"] = sorted_values[middle];
    }
    
    return stats;
}

nlohmann::json LogProcessor::analyze_by_user(const std::optional<DateRange>& date_range) {
    auto logs = load_logs(date_range);
    nlohmann::json result;
    
    // Count logs per user
    std::map<std::string, int> user_counts;
    std::map<std::string, std::vector<double>> response_times;
    
    for (const auto& log : logs) {
        user_counts[log.username]++;
        if (log.response_time > 0) {
            response_times[log.username].push_back(log.response_time);
        }
    }
    
    // Generate JSON with user statistics
    nlohmann::json users = nlohmann::json::array();
    for (const auto& [username, count] : user_counts) {
        nlohmann::json user;
        user["username"] = username;
        user["log_count"] = count;
        
        if (!response_times[username].empty()) {
            user["response_time_stats"] = calculate_statistics(response_times[username]);
        }
        
        users.push_back(user);
    }
    
    result["users"] = users;
    result["total_users"] = users.size();
    result["total_logs"] = logs.size();
    
    return result;
}

nlohmann::json LogProcessor::analyze_by_ip(const std::optional<DateRange>& date_range) {
    auto logs = load_logs(date_range);
    nlohmann::json result;
    
    // Count logs per IP
    std::map<std::string, int> ip_counts;
    std::map<std::string, std::vector<double>> response_times;
    
    for (const auto& log : logs) {
        ip_counts[log.ip_address]++;
        if (log.response_time > 0) {
            response_times[log.ip_address].push_back(log.response_time);
        }
    }
    
    // Generate JSON with IP statistics
    nlohmann::json ips = nlohmann::json::array();
    for (const auto& [ip, count] : ip_counts) {
        nlohmann::json ip_data;
        ip_data["ip_address"] = ip;
        ip_data["request_count"] = count;
        
        if (!response_times[ip].empty()) {
            ip_data["response_time_stats"] = calculate_statistics(response_times[ip]);
        }
        
        ips.push_back(ip_data);
    }
    
    result["ip_addresses"] = ips;
    result["unique_ips"] = ips.size();
    result["total_requests"] = logs.size();
    
    return result;
}

nlohmann::json LogProcessor::analyze_by_level(const std::optional<DateRange>& date_range) {
    auto logs = load_logs(date_range);
    nlohmann::json result;
    
    // Count logs per level
    std::map<std::string, int> level_counts;
    std::map<std::string, std::vector<double>> response_times;
    
    for (const auto& log : logs) {
        level_counts[log.log_level]++;
        if (log.response_time > 0) {
            response_times[log.log_level].push_back(log.response_time);
        }
    }
    
    // Generate JSON with level statistics
    nlohmann::json levels = nlohmann::json::array();
    for (const auto& [level, count] : level_counts) {
        nlohmann::json level_data;
        level_data["level"] = level;
        level_data["count"] = count;
        
        if (!response_times[level].empty()) {
            level_data["response_time_stats"] = calculate_statistics(response_times[level]);
        }
        
        levels.push_back(level_data);
    }
    
    result["log_levels"] = levels;
    result["total_logs"] = logs.size();
    
    return result;
}
