#include "TCPServer.hpp"
#include "TCPClient.hpp"
#include <nlohmann/json.hpp> // Ensure the JSON library is included
#include "LogProcessor.hpp"
#include <filesystem>
#include <cstdlib>
#include <iostream>

void ensure_logs() {
    if (!std::filesystem::exists("LogFiles")) {
#ifdef _WIN32
        system("extract_logs.bat");
    }
}

int main(int argc, char* argv[]) {
    ensure_logs();

    if (argc < 2) {
        std::cout << "Usage: NewApproachProgramming [server|client|process]\n";
        return 1;
    }

    std::string mode = argv[1];
    if (mode == "server") {
        TCPServer server(54000);
        server.start();
    }
    else if (mode == "client") {
        TCPClient client("127.0.0.1", 54000);
        client.start();
    }
    else if (mode == "process") {
        LogProcessor processor("LogFiles");
        processor.process_logs();
    }
    else {
        std::cerr << "Unknown mode: " << mode << "\n";
        return 1;
    }

    return 0;
}
