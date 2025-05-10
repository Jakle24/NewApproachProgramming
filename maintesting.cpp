#include <iostream>
#include "TCPServer.hpp"
#include "TCPClient.hpp"

int main() {
    int choice;
    std::cout << "1. Start server\n2. Start client\nChoose: ";
    std::cin >> choice;
    std::cin.ignore(); // clear newline

    if (choice == 1) {
        TCPServer server(54000);
        server.start();
    } else if (choice == 2) {
        std::string ip;
        std::cout << "Enter server IP (127.0.0.1 for local): ";
        std::cin >> ip;
        std::cin.ignore();
        TCPClient client(ip, 54000);
        client.start();
    } else {
        std::cout << "Invalid choice.\n";
    }
    return 0;
}