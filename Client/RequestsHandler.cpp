#include "RequestsHandler.h"
#include "FileHandler.h"
#include "Protocol.h"
#include "Endianness.h"


RequestsHandler::RequestsHandler(std::string client_id, std::string client_name, uint8_t client_version)
	: client_id(client_id), resolver(io_context), socket(io_context), client_name(client_name), client_version(client_version) {
	this->client_name.resize(CLIENT_NAME_SIZE, '\0'); // Resize and fill with null characters
	auto [server_ip, server_port, _, file_path] = read_transfer_file();
	if (!is_valid_ip(server_ip)) {
		throw std::runtime_error("\nnot a valid IP address from transfer.info file\n");
	}
	if (!is_valid_port(server_port))
		throw std::runtime_error("\n not a valid port from transfer.info file\n");
	boost::asio::connect(socket, resolver.resolve(server_ip, server_port));

	std::cout << "Connected successfully to IP: " << server_ip << ", Port: " << server_port << std::endl;

	std::cout << "Client name for the request_Handler: " << this->client_name << "Length: " << this->client_name.length() << std::endl;

}

bool RequestsHandler::is_valid_ip(std::string ip) {
	return true; // update this function laterd

}
bool RequestsHandler::is_valid_port(std::string port) {
	return true;  // update this function later
}

void RequestsHandler::send_register_request() {

	std::unique_ptr<Payload> payload = std::make_unique<RegisterPayload>(this->client_name);
	//RequestHeader(const std::string id, uint8_t version, uint16_t code, uint32_t payload_size);
	std::unique_ptr<Header> header = std::make_unique<RequestHeader>
		(this->client_id, this->client_version, REGISTER_REQUEST_CODE, payload->serialize().size());
	Packet packet(std::move(header), std::move(payload));

	std::vector<uint8_t> serialized_data = packet.serialize();

	std::cout << "Serialized Packet (hex): ";
	for (uint8_t byte : serialized_data) {
		printf("%02X ", byte);  // Print each byte in hex format
	}
	std::cout << std::endl;

	boost::asio::write(this->socket, boost::asio::buffer(serialized_data, serialized_data.size()));
	std::cout << "Serialized data sent to server!\n";

}
void RequestsHandler::receive_register_response() {
	// fill up the response parts : version, code, payload size.
	std::vector<uint8_t> response_buffer(SERVER_RESPONSE_HEADER_SIZE);
	boost::asio::read(this->socket, boost::asio::buffer(response_buffer, response_buffer.size()));

	// Extract server version (1 byte), response code (2 bytes), and payload size (4 bytes)

	uint8_t server_version = response_buffer[0];

	uint16_t response_code = response_buffer[1] | response_buffer[2] << 8;

	uint32_t payload_size = response_buffer[3] | response_buffer[4] << 8 
		| response_buffer[5] << 16 | response_buffer[6] << 24;

	// The sizes are in little endian, convert to native
	response_code = Endianness::from_little_to_native(response_code);
	payload_size = Endianness::from_little_to_native(payload_size);

	std::cout << "Received server version: " << static_cast<int>(server_version) << ", Response code: " << response_code
		<< " Payload size: " << payload_size << std::endl;
	if (response_code == REGISTER_FAIL_CODE) {
		throw std::runtime_error("\nRegistration failed. program will exit i guess \n");
	}

	std::vector<uint8_t> buf(payload_size);
	boost::asio::read(this->socket, boost::asio::buffer(buf, payload_size));
	this->client_id = std::string(buf.begin(), buf.end());
	std::cout << "Received client id: " << client_id << std::endl;

	// Create a .me file with this info

	outdata.open("fruits.txt", std::ios::app);


}

