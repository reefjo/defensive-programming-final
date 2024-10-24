#include "FileHandler.h"
#include <iostream>  // For std::cerr
#include <stdexcept> // For std::runtime_error
#include <iomanip> // for std::hex and std::setw

// Function to read server IP, port, client name, and file path from the transfer.info file
// Throws an exception if the file cannot be opened or read

std::tuple<std::string, std::string, std::string, std::string> read_transfer_file() {
	std::string filename = TRANSFER_FILE_NAME;
	std::ifstream transfer_file(filename);
	if (!transfer_file) {
		throw std::runtime_error("Couldn't read " + filename + " file");
	}
	// Variables to hold the extracted data
	std::string server_ip_port, client_name, file_path;

	// Read the lines from the transfer file (assumes file has exactly 3 lines)
	std::getline(transfer_file, server_ip_port);
	std::getline(transfer_file, client_name);
	std::getline(transfer_file, file_path);

	// Extract server IP and port from the first line
	std::string server_ip = server_ip_port.substr(0, server_ip_port.find(':'));
	std::string server_port = server_ip_port.substr(server_ip_port.find(':') + 1);

	return { server_ip, server_port, client_name, file_path };
}


// Function to write client name and client ID (as hex) into me.info
// Throws an exception if the file cannot be opened for writing
void write_me_info(const std::string client_name, const std::string client_id) {
	std::ofstream me_info_file("me.info");
	if (!me_info_file) {
		throw std::runtime_error("Failed to open me.info file for writing");
	}

	// Write client name to the first line
	me_info_file << client_name << std::endl;

	// Convert client ID to ASCII hex representation and write to the second line
	std::stringstream hex_representation;
	for (unsigned char c : client_id) {
		hex_representation << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(c);
	}
	// Write the ASCII hex representation of the client ID to the second line
	me_info_file << hex_representation.str() << std::endl;
	std::cout << "Succesfully wrote the me.info file\n";

	me_info_file.close();

}


// Function to write the private key to both the private key file and me.info (appended)
// Throws an exception if either file cannot be opened for writing
void put_key_in_files(std::string key) {
	// Write the key to the private key file
	std::ofstream key_file(PRIVATE_KEY_FILE_NAME);
	if (!key_file) {
		throw std::runtime_error("Failed to open private key file for writing");
	}
	key_file << key << std::endl;
	key_file.close();

	// Append the key to the me.info file (on the third line)
	std::ofstream me_info_file(ME_INFO_FILE_NAME, std::ios::app);
	if (!me_info_file) {
		throw std::runtime_error("Failed to open me.info file for writing");
	}
	me_info_file << key << std::endl; // Appends the key at the end (third line)
	me_info_file.close();

}

