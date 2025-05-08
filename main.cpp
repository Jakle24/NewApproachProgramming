#include "TCPServer.hpp"
#include "TCPClient.hpp"
#include "LogProcessor.hpp"
#include <iostream>

int main() {
    std::cout << "Select mode:\n";
    std::cout << "1. Server\n";
    std::cout << "2. Client\n";
    std::cout << "3. Log Processor\n";
    std::cout << "Choice: ";
    int choice;
    std::cin >> choice;
    std::cin.ignore(); // Clear newline

    if (choice == 1) {
        TCPServer server(54000);
        server.start();
    } else if (choice == 2) {
        TCPClient client("127.0.0.1", 54000);
        client.start();
    } else if (choice == 3) {
        std::string log_dir;
        std::cout << "Enter log directory: ";
        std::getline(std::cin, log_dir);
        LogProcessor processor(log_dir);
        processor.process_logs();
    } else {
        std::cout << "Invalid choice.\n";
    }
    return 0;
}