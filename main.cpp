#include "TCPServer.hpp"
#include "TCPClient.hpp"
#include "LogProcessor.hpp"
#include <iostream>
#include <filesystem>
//#include <minizip/unzip.h>

// Function to extract ZIP file
void extract_zip(const std::string& zip_path, const std::string& output_dir) {
    unzFile zip_file = unzOpen(zip_path.c_str());
    if (!zip_file) {
        std::cerr << "Failed to open ZIP file: " << zip_path << std::endl;
        return;
    }

    unz_global_info global_info;
    if (unzGetGlobalInfo(zip_file, &global_info) != UNZ_OK) {
        std::cerr << "Failed to get ZIP file info." << std::endl;
        unzClose(zip_file);
        return;
    }

    std::filesystem::create_directories(output_dir);

    for (uLong i = 0; i < global_info.number_entry; ++i) {
        char filename[256];
        unz_file_info file_info;
        if (unzGetCurrentFileInfo(zip_file, &file_info, filename, sizeof(filename), nullptr, 0, nullptr, 0) != UNZ_OK) {
            std::cerr << "Failed to get file info for entry " << i << std::endl;
            break;
        }

        std::string full_path = output_dir + "/" + filename;
        if (filename[file_info.size_filename - 1] == '/') {
            std::filesystem::create_directories(full_path);
        } else {
            if (unzOpenCurrentFile(zip_file) != UNZ_OK) {
                std::cerr << "Failed to open file in ZIP: " << filename << std::endl;
                break;
            }

            std::ofstream out_file(full_path, std::ios::binary);
            char buffer[8192];
            int bytes_read;
            while ((bytes_read = unzReadCurrentFile(zip_file, buffer, sizeof(buffer))) > 0) {
                out_file.write(buffer, bytes_read);
            }
            unzCloseCurrentFile(zip_file);
        }

        if (i + 1 < global_info.number_entry) {
            if (unzGoToNextFile(zip_file) != UNZ_OK) {
                std::cerr << "Failed to go to next file in ZIP." << std::endl;
                break;
            }
        }
    }

    unzClose(zip_file);
    std::cout << "Extraction complete." << std::endl;
}

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
        std::string zip_path = "C:\\Users\\Student\\Downloads\\logs.zip";
        std::string output_dir = "LogFiles";

        // Extract the ZIP file
        extract_zip(zip_path, output_dir);

        // Process the logs
        LogProcessor processor(output_dir);
        processor.process_logs();
    } else {
        std::cout << "Invalid choice.\n";
    }
    return 0;
}