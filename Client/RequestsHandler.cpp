#include "RequestsHandler.h"
#include "FileHandler.h"
#include "Protocol.h"
#include "Endianness.h"
RequestsHandler::RequestsHandler(std::string client_name, uint8_t client_version)
	: resolver(io_context), socket(io_context), client_name(client_name), client_version(client_version) {

	auto [server_ip, server_port, _, file_path] = read_transfer_file();
	if (!is_valid_ip(server_ip)) {
		throw std::runtime_error("\nnot a valid IP address from transfer.info file\n");
	}
	if (!is_valid_port(server_port))
		throw std::runtime_error("\n not a valid port from transfer.info file\n");
	boost::asio::connect(socket, resolver.resolve(server_ip, server_port));

	std::cout << "Connected successfully to IP: " << server_ip << ", Port: " << server_port << std::endl;

	// Initialize client_name to a size of 255
	this->client_name.resize(CLIENT_NAME_SIZE, '\0'); // Resize and fill with null characters
	std::cout << "Client name for the request_Handler: " << this->client_name << "Length: " << this->client_name.length() << std::endl;


}

bool RequestsHandler::is_valid_ip(std::string ip) {
	return true; // update this function later

}
bool RequestsHandler::is_valid_port(std::string port) {
	return true;  // update this function later
}

void RequestsHandler::send_register_request() {
	std::vector<uint8_t> arr;
	load_id_version(arr);
	// load the register code
	load_code(arr, REGISTER_CODE);
	load_payload_size(arr, REGISTER_REQUEST_SIZE);
	// Load the client name (char by char)
	for (char c : client_name) {
		arr.push_back(static_cast<uint8_t>(c));  // Convert each char to uint8_t and push
	}

	boost::asio::write(this->socket, boost::asio::buffer(arr, arr.size()));

}

void RequestsHandler::load_code(std::vector<uint8_t>& arr, uint16_t code) {
	uint16_t code_little_endian = Endianness::to_little_endian(code);

	// Push back each byte independently
	arr.push_back(static_cast<uint8_t>(code_little_endian & 0xFF));      // Lower byte
	arr.push_back(static_cast<uint8_t>((code_little_endian >> 8) & 0xFF)); // Upper byte
}
void RequestsHandler::load_payload_size(std::vector<uint8_t>& arr, uint32_t size) {
	uint32_t size_little_endian = Endianness::to_little_endian(size);
	arr.push_back(static_cast<uint8_t>(size_little_endian & 0xFF));          // Byte 0
	arr.push_back(static_cast<uint8_t>((size_little_endian >> 8) & 0xFF));   // Byte 1
	arr.push_back(static_cast<uint8_t>((size_little_endian >> 16) & 0xFF));  // Byte 2
	arr.push_back(static_cast<uint8_t>((size_little_endian >> 24) & 0xFF));  // Byte 3
}
void RequestsHandler::load_id_version(std::vector < uint8_t> &arr) {
	// load the client id (if exists. and if not - load with junk) up to 16 bytes

	// load the client id
	for (int i = 0; i < ID_SIZE; i++) {
			arr.push_back(this->client_id.at(i));;
	}
	// load the version
	arr.push_back(this->client_version);

	

}