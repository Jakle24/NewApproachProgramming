// save as process_all_logs.cpp
#include <iostream>
#include <filesystem>
#include <string>
#include "src/LogProcessor.hpp"

namespace fs = std::filesystem;

int main() {
    LogProcessor processor(".");
    
    // Path to extracted logs
    std::string logsPath = "Logs";
    
    // Count for reporting
    int jsonCount = 0, txtCount = 0, xmlCount = 0;
    
    // Process all files in the directory
    for (const auto& entry : fs::recursive_directory_iterator(logsPath)) {
        if (entry.is_regular_file()) {
            std::string path = entry.path().string();
            std::string ext = entry.path().extension().string();
            
            std::vector<LogEntry> entries;
            
            if (ext == ".json") {
                entries = processor.parse_json(path);
                jsonCount += entries.size();
                std::cout << "Processed JSON file: " << path << " (" << entries.size() << " entries)\n";
            } 
            else if (ext == ".txt") {
                entries = processor.parse_txt(path);
                txtCount += entries.size();
                std::cout << "Processed TXT file: " << path << " (" << entries.size() << " entries)\n";
            }
            else if (ext == ".xml") {
                entries = processor.parse_xml(path);
                xmlCount += entries.size();
                std::cout << "Processed XML file: " << path << " (" << entries.size() << " entries)\n";
            }
        }
    }
    
    std::cout << "\n=== Summary ===\n";
    std::cout << "JSON entries: " << jsonCount << "\n";
    std::cout << "TXT entries: " << txtCount << "\n";
    std::cout << "XML entries: " << xmlCount << "\n";
    std::cout << "Total: " << (jsonCount + txtCount + xmlCount) << " entries\n";
    
    return 0;
}