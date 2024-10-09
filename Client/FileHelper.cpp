#include "FileHelper.h"

// Function to read server IP, port, client name, and file path from info.transfer file
std::tuple<std::string, std::string, std::string, std::string> readTransferFile() {
    auto filename = TRANSFER_FILE_NAME;
    std::ifstream transfer_file(filename);
    if (!transfer_file) {
        throw std::runtime_error("Couldn't read " + filename + " file");
    }
    // Read the lines from the file
    std::string server_ip_port, client_name, file_path;
    std::getline(transfer_file, server_ip_port);
    std::getline(transfer_file, client_name);
    std::getline(transfer_file, file_path);

    // Extract server IP and port
    std::string server_ip = server_ip_port.substr(0, server_ip_port.find(':'));
    std::string server_port = server_ip_port.substr(server_ip_port.find(':') + 1);

    return { server_ip, server_port, client_name, file_path };
}