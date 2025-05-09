#include "LogProcessor.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <regex>

namespace fs = std::filesystem;

// Helper: Calculate statistics
void print_statistics(const std::vector<double>& numbers) {
    if (numbers.empty()) {
        std::cout << "No numeric data found.\n";
        return;
    }
    double sum = std::accumulate(numbers.begin(), numbers.end(), 0.0);
    double avg = sum / numbers.size();
    double min = *std::min_element(numbers.begin(), numbers.end());
    double max = *std::max_element(numbers.begin(), numbers.end());
    std::vector<double> sorted = numbers;
    std::sort(sorted.begin(), sorted.end());
    double median = sorted.size() % 2 == 0 ?
        (sorted[sorted.size()/2 - 1] + sorted[sorted.size()/2]) / 2.0 :
        sorted[sorted.size()/2];

    std::cout << "Count: " << numbers.size() << "\n";
    std::cout << "Average: " << avg << "\n";
    std::cout << "Min: " << min << "\n";
    std::cout << "Max: " << max << "\n";
    std::cout << "Median: " << median << "\n";
}

// Helper: Try to extract a number from a string
bool extract_number(const std::string& token, double& number) {
    try {
        size_t idx;
        number = std::stod(token, &idx);
        return idx == token.size();
    } catch (...) {
        return false;
    }
}

// CSV parser
std::vector<double> parse_csv(const std::string& filepath) {
    std::vector<double> numbers;
    std::ifstream file(filepath);
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        while (std::getline(ss, cell, ',')) {
            double num;
            if (extract_number(cell, num)) {
                numbers.push_back(num);
            }
        }
    }
    return numbers;
}

// Plain text parser (numbers separated by whitespace)
std::vector<double> parse_plain(const std::string& filepath) {
    std::vector<double> numbers;
    std::ifstream file(filepath);
    std::string token;
    while (file >> token) {
        double num;
        if (extract_number(token, num)) {
            numbers.push_back(num);
        }
    }
    return numbers;
}

// JSON parser (very basic, looks for numbers in the file)
std::vector<double> parse_json(const std::string& filepath) {
    std::vector<double> numbers;
    std::ifstream file(filepath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::regex num_regex(R"([-+]?\d*\.?\d+([eE][-+]?\d+)?)");
    auto nums_begin = std::sregex_iterator(content.begin(), content.end(), num_regex);
    auto nums_end = std::sregex_iterator();
    for (auto it = nums_begin; it != nums_end; ++it) {
        double num;
        if (extract_number(it->str(), num)) {
            numbers.push_back(num);
        }
    }
    return numbers;
}

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
    std::cout << "Processing file: " << filepath << std::endl;
    std::vector<double> numbers;
    if (filepath.size() >= 4 && filepath.compare(filepath.size() - 4, 4, ".csv") == 0) {
        numbers = parse_csv(filepath);
    } else if (filepath.size() >= 5 && filepath.compare(filepath.size() - 5, 5, ".json") == 0) {
        numbers = parse_json(filepath);
    } else {
        numbers = parse_plain(filepath);
    }
    print_statistics(numbers);
}

void LogProcessor::process_logs() {
    auto files = get_log_files();
    for (const auto& file : files) {
        process_log_file(file);
        std::cout << "-----------------------------\n";
    }
}
