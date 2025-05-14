// test_parser.cpp - Simplified log parser to avoid Norton issues
#include <iostream>
#include <string>
#include <vector>
#include "src/LogEntry.hpp"
#include "src/LogProcessor.hpp"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: test_parser <file_path> <file_type>\n";
        std::cout << "  file_type can be: json, txt, or xml\n";
        return 1;
    }

    std::string file_path = argv[1];
    std::string file_type = argv[2];
    
    LogProcessor processor(".");
    std::vector<LogEntry> entries;
    
    // Parse based on file type
    if (file_type == "json") {
        entries = processor.parse_json(file_path);
    } else if (file_type == "txt") {
        entries = processor.parse_txt(file_path);
    } else if (file_type == "xml") {
        entries = processor.parse_xml(file_path);
    } else {
        std::cout << "Unknown file type: " << file_type << "\n";
        return 1;
    }
    
    // Display parsed entries
    std::cout << "Successfully parsed " << entries.size() << " entries.\n\n";
    
    for (const auto& entry : entries) {
        auto time_t_point = std::chrono::system_clock::to_time_t(entry.timestamp);
        std::tm tm = {};
        localtime_s(&tm, &time_t_point);
        
        std::cout << "------------------------------------\n";
        std::cout << "Timestamp: " << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "\n";
        std::cout << "Username: " << entry.username << "\n";
        std::cout << "IP Address: " << entry.ip_address << "\n";
        std::cout << "Log Level: " << entry.log_level << "\n";
        std::cout << "Message: " << entry.message << "\n";
        std::cout << "Response Time: " << entry.response_time << " ms\n";
    }
    
    return 0;
}