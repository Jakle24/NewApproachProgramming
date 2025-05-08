#include "LogProcessor.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

LogProcessor::LogProcessor(const std::string& log_dir) : log_dir_(log_dir) {}

std::vector<std::string> LogProcessor::get_log_files() {
    std::vector<std::string> files;
    for (const auto& entry : fs::directory_iterator(log_dir_)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path().string());
        }
    }
    return files;
}

void LogProcessor::process_log_file(const std::string& filepath) {
    std::ifstream file(filepath);
    std::string line;
    while (std::getline(file, line)) {
        // Process each line (parse, analyze, etc.)
        std::cout << "Processing: " << line << std::endl;
    }
}

void LogProcessor::process_logs() {
    auto files = get_log_files();
    for (const auto& file : files) {
        process_log_file(file);
    }
}