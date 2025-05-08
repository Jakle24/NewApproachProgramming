#pragma once
#include <string>
#include <vector>

class LogProcessor {
public:
    LogProcessor(const std::string& log_dir);
    void process_logs();
    // Add more methods as needed

private:
    std::string log_dir_;
    std::vector<std::string> get_log_files();
    void process_log_file(const std::string& filepath);
};