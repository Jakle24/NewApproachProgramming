#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <nlohmann/json.hpp>
#include "TCPServer.hpp"
#include "TCPClient.hpp"

/**
 * @brief Displays usage instructions for the application
 * 
 * Shows available command-line options and parameters for both server and client modes.
 */
void print_usage() {
    std::cout << "Usage:" << std::endl;
    std::cout << "  server                          Start the server" << std::endl;
    std::cout << "  client --log-folder <folder> --analysis <type> [--start <date>] [--end <date>]" << std::endl;
    std::cout << "    <folder>: Path to the log files folder" << std::endl;
    std::cout << "    <type>: Analysis type (user, ip, or level)" << std::endl;
    std::cout << "    <date>: Optional date range in format 'YYYY-MM-DD HH:MM:SS'" << std::endl;
}

/**
 * @brief Formats and displays statistical data in a readable format
 * @param stats JSON object containing statistical measures
 * 
 * Presents count, minimum, maximum, average, and median values.
 */
void format_statistics(const nlohmann::json& stats) {
    std::cout << "  Count: " << stats["count"].get<int>() << std::endl;
    std::cout << "  Min: " << stats["min"].get<double>() << std::endl;
    std::cout << "  Max: " << stats["max"].get<double>() << std::endl;
    std::cout << "  Average: " << stats["average"].get<double>() << std::endl;
    std::cout << "  Median: " << stats["median"].get<double>() << std::endl;
}

/**
 * @brief Entry point for server mode operation
 * 
 * Initializes and starts the TCP server to handle client connections.
 */
void run_server() {
    TCPServer server(8080);
    server.start();
}

/**
 * @brief Entry point for client mode operation
 * @param log_folder Directory containing log files
 * @param analysis_type Type of analysis to perform (user/ip/level)
 * @param start_date Optional start of date range filter
 * @param end_date Optional end of date range filter
 * 
 * Connects to the server, sends the analysis request with parameters,
 * receives results, and displays them in a formatted manner.
 */
void run_client(const std::string& log_folder, const std::string& analysis_type,
                const std::string& start_date = "", const std::string& end_date = "") {
    
    TCPClient client("127.0.0.1", 8080);
    
    // Prepare request
    nlohmann::json request;
    request["analysis_type"] = analysis_type;
    request["log_folder"] = log_folder;
    
    if (!start_date.empty() && !end_date.empty()) {
        request["start_date"] = start_date;
        request["end_date"] = end_date;
    }
    
    std::cout << "Sending request to server..." << std::endl;
    
    // Send request and get response
    nlohmann::json response = client.send_request(request);
    
    if (response.contains("error")) {
        std::cerr << "Error: " << response["error"].get<std::string>() << std::endl;
        return;
    }
    
    // Display results based on analysis type
    std::cout << "=== Analysis Results ===" << std::endl;
    
    if (analysis_type == "user") {
        std::cout << "Total Users: " << response["total_users"].get<int>() << std::endl;
        std::cout << "Total Logs: " << response["total_logs"].get<int>() << std::endl;
        
        std::cout << "\nUser Statistics:" << std::endl;
        for (const auto& user : response["users"]) {
            std::cout << "\nUsername: " << user["username"].get<std::string>() << std::endl;
            std::cout << "Log Count: " << user["log_count"].get<int>() << std::endl;
            
            if (user.contains("response_time_stats")) {
                std::cout << "Response Time Statistics:" << std::endl;
                format_statistics(user["response_time_stats"]);
            }
        }
    }
    else if (analysis_type == "ip") {
        std::cout << "Unique IPs: " << response["unique_ips"].get<int>() << std::endl;
        std::cout << "Total Requests: " << response["total_requests"].get<int>() << std::endl;
        
        std::cout << "\nIP Address Statistics:" << std::endl;
        for (const auto& ip : response["ip_addresses"]) {
            std::cout << "\nIP: " << ip["ip_address"].get<std::string>() << std::endl;
            std::cout << "Request Count: " << ip["request_count"].get<int>() << std::endl;
            
            if (ip.contains("response_time_stats")) {
                std::cout << "Response Time Statistics:" << std::endl;
                format_statistics(ip["response_time_stats"]);
            }
        }
    }
    else if (analysis_type == "level") {
        std::cout << "Total Logs: " << response["total_logs"].get<int>() << std::endl;
        
        std::cout << "\nLog Level Statistics:" << std::endl;
        for (const auto& level : response["log_levels"]) {
            std::cout << "\nLevel: " << level["level"].get<std::string>() << std::endl;
            std::cout << "Count: " << level["count"].get<int>() << std::endl;
            
            if (level.contains("response_time_stats")) {
                std::cout << "Response Time Statistics:" << std::endl;
                format_statistics(level["response_time_stats"]);
            }
        }
    }
}

/**
 * @brief Application entry point
 * @param argc Number of command-line arguments
 * @param argv Array of command-line argument strings
 * @return Exit code (0 for success, non-zero for errors)
 * 
 * Parses command-line arguments, determines operation mode (server or client),
 * and delegates to the appropriate function.
 */
int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }
    
    std::string mode = argv[1];
    
    if (mode == "server") {
        std::cout << "Starting server mode..." << std::endl;
        run_server();
    }
    else if (mode == "client") {
        std::string log_folder;
        std::string analysis_type;
        std::string start_date;
        std::string end_date;
        
        // Parse client arguments
        for (int i = 2; i < argc; i++) {
            std::string arg = argv[i];
            
            if (arg == "--log-folder" && i + 1 < argc) {
                log_folder = argv[++i];
            }
            else if (arg == "--analysis" && i + 1 < argc) {
                analysis_type = argv[++i];
            }
            else if (arg == "--start" && i + 1 < argc) {
                start_date = argv[++i];
            }
            else if (arg == "--end" && i + 1 < argc) {
                end_date = argv[++i];
            }
        }
        
        // Validate required parameters
        if (log_folder.empty() || analysis_type.empty()) {
            std::cerr << "Error: Missing required parameters." << std::endl;
            print_usage();
            return 1;
        }
        
        // Validate analysis type
        if (analysis_type != "user" && analysis_type != "ip" && analysis_type != "level") {
            std::cerr << "Error: Invalid analysis type. Must be 'user', 'ip', or 'level'." << std::endl;
            return 1;
        }
        
        run_client(log_folder, analysis_type, start_date, end_date);
    }
    else {
        std::cerr << "Invalid mode: " << mode << std::endl;
        print_usage();
        return 1;
    }
    
    return 0;
}
