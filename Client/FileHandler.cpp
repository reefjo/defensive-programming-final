#include "FileHandler.h"
#include <iostream>  // For std::cerr
#include <stdexcept> // For std::runtime_error
#include <iomanip> // for std::hex and std::setw

#define BUFFER_SIZE 1024

// Initialize the global variable for endianness









// Function to read server IP, port, client name, and file path from info.transfer file
std::tuple<std::string, std::string, std::string, std::string> read_transfer_file() {
	std::string filename = TRANSFER_FILE_NAME;
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

void write_me_info(const std::string client_name, const std::string client_id) {
	std::ofstream me_info_file("me.info");
	if (!me_info_file) {
		throw std::runtime_error("Failed to open me.info file for writing");
	}

	// Write client name to the first line
	me_info_file << client_name << std::endl;

	// Convert client_id to its ASCII hex representation (2 hex characters per byte)
	std::stringstream hex_representation;
	for (unsigned char c : client_id) {
		hex_representation << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(c);
	}
	// Write the ASCII hex representation of the client ID to the second line
	me_info_file << hex_representation.str() << std::endl;
	std::cout << "Succesfully wrote the me.info file\n";

	me_info_file.close();

}

void put_key_in_files(std::string key) {
	// puts the key in file, not the ASCII representation
	std::ofstream key_file(PRIVATE_KEY_FILE_NAME);
	if (!key_file) {
		throw std::runtime_error("Failed to open private key file for writing");
	}
	key_file << key << std::endl;
	key_file.close();
	std::ofstream me_info_file(ME_INFO_FILE_NAME, std::ios::app);
	if (!me_info_file) {
		throw std::runtime_error("Failed to open me.info file for writing");
	}
	me_info_file << key << std::endl; // Appends the key at the end (third line)
	me_info_file.close();

}
std::vector<std::string> read_lines_from_file(const std::string& file_name) {
	std::vector<std::string> res;
	std::ifstream file(file_name);

	if (!file) {
		return res;  // File doesn't exist, return empty vector
	}

	std::string line;
	while (std::getline(file, line)) {
		res.push_back(line);
	}

	file.close();
	return res;
}
/*
std::string get_file_from_connection() {
	return "";

}
// Function to send file to server using Boost Asio
void FileHandler::sendFile(const std::string& fileName, boost::asio::ip::tcp::socket& socket) {

	std::ifstream file(fileName, std::ios::binary);
	if (!file) {
		std::cerr << "Error opening file: " << fileName << std::endl;
		return;
	}

	// Get the file size
	file.seekg(0, std::ios::end);
	std::streamsize fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	// Prepare buffer for size in little-endian format
	char sizeBuffer[4];
	// Prepare size as 4 bytes (uint32_t)
	uint32_t fileSizeLE = convert_to_little(static_cast<uint32_t>(fileSize));
	memcpy(sizeBuffer, &fileSizeLE, sizeof(fileSizeLE)); // Pack the size into 4 bytes


	// Send the file size as the first 4 bytes in little-endian format
	boost::asio::write(socket, boost::asio::buffer(sizeBuffer, sizeof(sizeBuffer)));

	char buffer[BUFFER_SIZE];
	while (file.read(buffer, sizeof(buffer))) {
		boost::asio::write(socket, boost::asio::buffer(buffer, sizeof(buffer)));
	}

	if (file.gcount() > 0) {
		boost::asio::write(socket, boost::asio::buffer(buffer, file.gcount()));
	}


	file.close();
	std::cout << "File sent successfully!" << std::endl;
}

// Function to receive file from server using Boost Asio
void FileHandler::receiveFile(const std::string& fileName, boost::asio::ip::tcp::socket& socket) {
	std::ofstream file(fileName, std::ios::binary);
	if (!file) {
		std::cerr << "Error opening file for writing: " << fileName << std::endl;
		return;
	}

	char buffer[BUFFER_SIZE];
	size_t bytesRead;

	while ((bytesRead = socket.read_some(boost::asio::buffer(buffer, BUFFER_SIZE))) > 0) {
		std::string data(buffer, bytesRead);
		if (data.find("EOF") != std::string::npos) {
			data = data.substr(0, data.find("EOF"));
			file.write(data.c_str(), data.size());
			break;
		}
		file.write(buffer, bytesRead);
	}

	file.close();
	std::cout << "File received successfully!" << std::endl;
}

*/


