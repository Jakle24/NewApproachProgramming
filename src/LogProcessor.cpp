#include "LogProcessor.hpp"
#include "LogEntry.hpp"
#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <algorithm>
#include <numeric>
#include <thread>
#include <mutex>

namespace fs = std::filesystem;
using json = nlohmann::json;

// Helper: List files in the log directory
std::vector<std::string> LogProcessor::get_log_files() {
    std::vector<std::string> files;
    for (const auto& entry : fs::directory_iterator(log_folder)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path().string());
        }
    }
    return files;
}

// Helper: Parse TXT/CSV logs
std::vector<LogEntry> LogProcessor::parse_txt(const std::string& file_path) {
    std::vector<LogEntry> entries;
    std::ifstream file(file_path);
    
    if (!file.is_open()) {
        std::cerr << "Failed to open: " << file_path << std::endl;
        return entries;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        auto entry_opt = LogEntry::parse_log_line(line);
        if (entry_opt) {
            entries.push_back(*entry_opt);
        }
    }
    
    return entries;
}

std::vector<LogEntry> LogProcessor::parse_json(const std::string& filepath) {
    std::vector<LogEntry> entries;
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        std::cerr << "Failed to open JSON file: " << filepath << std::endl;
        return entries;
    }
    
    try {
        nlohmann::json j;
        file >> j;
        
        // Handle single object format
        if (!j.is_array() && !j.contains("logs")) {
            // Process as a single log entry
            if (j.contains("username") && j.contains("ip_address") &&
                j.contains("log_level") && j.contains("timestamp")) {
                
                LogEntry entry;
                entry.timestamp = LogEntry::parse_timestamp(j["timestamp"]);
                entry.username = j["username"];
                entry.ip_address = j["ip_address"];
                entry.log_level = j["log_level"];
                entry.message = j.value("message", "");
                entry.response_time = j.value("response_time", 0.0);
                
                entries.push_back(entry);
                std::cout << "Successfully parsed single JSON log entry" << std::endl;
                return entries;
            }
        }
        
        // Handle array inside "logs" property
        if (j.contains("logs") && j["logs"].is_array()) {
            for (const auto& log : j["logs"]) {
                if (log.contains("username") && log.contains("ip_address") &&
                    log.contains("log_level") && log.contains("timestamp")) {
                    
                    LogEntry entry;
                    entry.timestamp = LogEntry::parse_timestamp(log["timestamp"]);
                    entry.username = log["username"];
                    entry.ip_address = log["ip_address"];
                    entry.log_level = log["log_level"];
                    entry.message = log.value("message", "");
                    entry.response_time = log.value("response_time", 0.0);
                    
                    entries.push_back(entry);
                }
            }
            
            std::cout << "Successfully parsed " << entries.size() << " logs from JSON file" << std::endl;
        }
        
        // Handle direct array format (if needed)
        else if (j.is_array()) {
            for (const auto& log : j) {
                if (log.contains("username") && log.contains("ip_address") &&
                    log.contains("log_level") && log.contains("timestamp")) {
                    
                    LogEntry entry;
                    entry.timestamp = LogEntry::parse_timestamp(log["timestamp"]);
                    entry.username = log["username"];
                    entry.ip_address = log["ip_address"];
                    entry.log_level = log["log_level"];
                    entry.message = log.value("message", "");
                    entry.response_time = log.value("response_time", 0.0);
                    
                    entries.push_back(entry);
                }
            }
            
            std::cout << "Successfully parsed " << entries.size() << " logs from JSON array" << std::endl;
        }
        else {
            std::cerr << "Invalid JSON structure in " << filepath << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error parsing JSON file " << filepath << ": " << e.what() << std::endl;
    }
    
    return entries;
}


// Implement in LogProcessor.cpp
std::vector<LogEntry> LogProcessor::parse_xml(const std::string& filepath) {
    std::vector<LogEntry> entries;
    std::cout << "Parsing XML file: " << filepath << std::endl;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open XML file: " << filepath << std::endl;
        return entries;
    }
    
    // Read the entire file content
    std::string xml_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Basic XML parsing - extract log entries
    size_t pos = 0;
    while ((pos = xml_content.find("<log>", pos)) != std::string::npos) {
        size_t end_pos = xml_content.find("</log>", pos);
        if (end_pos == std::string::npos) break;
        
        std::string log_entry = xml_content.substr(pos, end_pos - pos + 6);
        
        // Extract fields
        std::string username = extract_xml_tag(log_entry, "username");
        std::string ip = extract_xml_tag(log_entry, "ip_address");
        std::string level = extract_xml_tag(log_entry, "log_level");
        std::string timestamp_str = extract_xml_tag(log_entry, "timestamp");
        std::string message = extract_xml_tag(log_entry, "message");
        
        if (!username.empty() && !ip.empty() && !level.empty() && !timestamp_str.empty()) {
            LogEntry entry;
            entry.username = username;
            entry.ip_address = ip;
            entry.log_level = level;
            entry.timestamp = LogEntry::parse_timestamp(timestamp_str);
            entry.message = message;
            
            std::string response_time_str = extract_xml_tag(log_entry, "response_time");
            if (!response_time_str.empty()) {
                try {
                    entry.response_time = std::stod(response_time_str);
                } catch (...) {
                    entry.response_time = 0.0;
                }
            }
            
            entries.push_back(entry);
        }
        
        pos = end_pos + 6; // Move past </log>
    }
    
    std::cout << "Extracted " << entries.size() << " entries from XML" << std::endl;
    return entries;
}

// Helper function for XML parsing
std::string LogProcessor::extract_xml_tag(const std::string& xml, const std::string& tag) {
    std::string open_tag = "<" + tag + ">";
    std::string close_tag = "</" + tag + ">";
    
    size_t start_pos = xml.find(open_tag);
    if (start_pos == std::string::npos) return "";
    
    start_pos += open_tag.length();
    size_t end_pos = xml.find(close_tag, start_pos);
    if (end_pos == std::string::npos) return "";
    
    return xml.substr(start_pos, end_pos - start_pos);
}

// Analyze entries (by user, IP, level)
void LogProcessor::analyze(const std::vector<LogEntry>& entries) {
    std::map<std::string, int> by_user;
    std::map<std::string, int> by_ip;
    std::map<std::string, int> by_level;

    for (const auto& e : entries) {
        by_user[e.username]++;
        by_ip[e.ip_address]++;
        by_level[e.log_level]++;
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

std::vector<LogEntry> LogProcessor::process_logs_parallel(const std::optional<DateRange>& date_range) {
    std::vector<LogEntry> all_logs;
    std::vector<std::string> file_paths;
    std::mutex logs_mutex;
    
    // First, collect all file paths
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(log_folder)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                if (ext == ".txt" || ext == ".json" || ext == ".xml") {
                    file_paths.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error scanning directory: " << e.what() << std::endl;
        return all_logs;
    }
    
    std::cout << "Found " << file_paths.size() << " log files to process in parallel" << std::endl;
    
    // Process files in parallel
    std::vector<std::thread> threads;
    for (const auto& path : file_paths) {
        threads.push_back(std::thread([&, path]() {
            std::vector<LogEntry> file_logs;
            std::string ext = std::filesystem::path(path).extension().string();
            
            if (ext == ".txt") {
                file_logs = parse_txt(path);
            } else if (ext == ".json") {
                file_logs = parse_json(path);
            } else if (ext == ".xml") {
                file_logs = parse_xml(path);
            }
            
            // Filter by date if needed
            if (date_range) {
                std::vector<LogEntry> filtered_logs;
                for (const auto& log : file_logs) {
                    if (log.timestamp >= date_range->start && log.timestamp <= date_range->end) {
                        filtered_logs.push_back(log);
                    }
                }
                file_logs = filtered_logs;
            }
            
            // Add to global logs vector (thread-safe)
            {
                std::lock_guard<std::mutex> lock(logs_mutex);
                all_logs.insert(all_logs.end(), file_logs.begin(), file_logs.end());
                std::cout << "Thread finished processing " << path << " with " << file_logs.size() << " entries" << std::endl;
            }
        }));
    }
    
    // Wait for all threads to finish
    for (auto& thread : threads) {
        thread.join();
    }
    
    std::cout << "Loaded " << all_logs.size() << " log entries using parallel processing" << std::endl;
    return all_logs;
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
    auto logs = process_logs_parallel(date_range);
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
    auto logs = process_logs_parallel(date_range);
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
    auto logs = process_logs_parallel(date_range);
    nlohmann::json result;
    
    // Count logs per log level
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
        level_data["log_level"] = level;
        level_data["count"] = count;
        
        if (!response_times[level].empty()) {
            level_data["response_time_stats"] = calculate_statistics(response_times[level]);
        }
        
        levels.push_back(level_data);
    }
    
    result["log_levels"] = levels;
    result["total_levels"] = levels.size();
    result["total_logs"] = logs.size();
    
    return result;
}
