#include <iostream>
#include <filesystem>
#include <minizip/unzip.h>

void extract_zip(const std::string& zip_path, const std::string& output_dir) {
    unzFile zip_file = unzOpen(zip_path.c_str());
    if (!zip_file) {
        std::cerr << "ERROR: Failed to open ZIP file: " << zip_path << std::endl;
        return;
    }

    unz_global_info global_info;
    if (unzGetGlobalInfo(zip_file, &global_info) != UNZ_OK) {
        std::cerr << "ERROR: Failed to get ZIP file info." << std::endl;
        unzClose(zip_file);
        return;
    }

    std::filesystem::create_directories(output_dir);

    for (uLong i = 0; i < global_info.number_entry; ++i) {
        char filename[256];
        unz_file_info file_info;
        if (unzGetCurrentFileInfo(zip_file, &file_info, filename, sizeof(filename), nullptr, 0, nullptr, 0) != UNZ_OK) {
            std::cerr << "ERROR: Failed to get file info for entry " << i << std::endl;
            break;
        }

        std::string full_path = output_dir + "/" + filename;
        if (filename[file_info.size_filename - 1] == '/') {
            std::filesystem::create_directories(full_path);
        } else {
            if (unzOpenCurrentFile(zip_file) != UNZ_OK) {
                std::cerr << "ERROR: Failed to open file in ZIP: " << filename << std::endl;
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
                std::cerr << "ERROR: Failed to go to next file in ZIP." << std::endl;
                break;
            }
        }
    }

    unzClose(zip_file);
    std::cout << "Extraction complete. Logs are in the " << output_dir << " directory." << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <zip_file> <output_dir>" << std::endl;
        return 1;
    }

    std::string zip_path = argv[1];
    std::string output_dir = argv[2];

    extract_zip(zip_path, output_dir);
    return 0;
}