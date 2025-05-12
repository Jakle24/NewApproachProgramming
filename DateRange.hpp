#pragma once
#include <chrono>

struct DateRange {
    std::chrono::system_clock::time_point start;
    std::chrono::system_clock::time_point end;
};