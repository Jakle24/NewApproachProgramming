#include "src/LogProcessor.hpp"
#include <iostream>
#include <nlohmann/json.hpp>

int main() {
    // Create a LogProcessor instance
    LogProcessor processor(".");  // Current directory, not used for direct file parsing
    
    // Path to your JSON file
    std::string json_path = "TestLog/single_log.json";
    
    // Parse the JSON file
    std::vector<LogEntry> logs = processor.parse_json(json_path);
    
    // Check if parsing was successful
    if (logs.empty()) {
        std::cout << "Failed to parse any logs from the file." << std::endl;
        return 1;
    }
    
    // Display the parsed log entry
    std::cout << "Successfully parsed " << logs.size() << " log entries." << std::endl;
    
    // Display the first log entry's details
    const LogEntry& log = logs[0];
    auto time_t_point = std::chrono::system_clock::to_time_t(log.timestamp);
    std::tm tm = {};
    localtime_s(&tm, &time_t_point);
    std::cout << "Timestamp: ";
    std::cout << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << std::endl;
    std::cout << "Username: " << log.username << std::endl;
    std::cout << "IP Address: " << log.ip_address << std::endl;
    std::cout << "Log Level: " << log.log_level << std::endl;
    std::cout << "Message: " << log.message << std::endl;
    std::cout << "Response Time: " << log.response_time << " ms" << std::endl;
    
    return 0;
}