#include "FileHandler.h"
#include <fstream>   // For std::ifstream, std::ofstream
#include <iostream>  // For std::cerr
#include <stdexcept> // For std::runtime_error

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


