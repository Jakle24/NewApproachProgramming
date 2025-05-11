#ifndef LOG_ENTRY_HPP
#define LOG_ENTRY_HPP

#include <string>
#include <chrono>

struct LogEntry {
    std::string user;
    std::string ip;
    std::string level;
    std::string timestamp; // Raw string
    std::chrono::system_clock::time_point parsed_time;

    // Optional: helper to parse timestamp on creation
    bool parse_timestamp();
};

#endif
