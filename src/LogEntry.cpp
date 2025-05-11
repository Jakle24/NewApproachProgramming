#include "LogEntry.hpp"
#include <iomanip>
#include <sstream>

// Very basic timestamp parser (assumes "YYYY-MM-DD HH:MM:SS")
bool LogEntry::parse_timestamp() {
    std::tm tm = {};
    std::istringstream ss(timestamp);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) return false;
    parsed_time = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    return true;
}
