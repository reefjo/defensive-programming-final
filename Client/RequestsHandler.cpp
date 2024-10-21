#include "RequestsHandler.h"
#include "FileHandler.h"
#include "Protocol.h"
#include "Endianness.h"


RequestsHandler::RequestsHandler(std::string cid, std::string cname, uint8_t cversion)
	: client_id(cid), resolver(io_context), socket(io_context), client_name(cname), client_version(cversion) {
	//this->client_name.resize(CLIENT_NAME_SIZE, '\0'); // Resize and fill with null characters
	auto [server_ip, server_port, _, file_path] = read_transfer_file();
	if (!is_valid_ip(server_ip)) {
		throw std::runtime_error("\nnot a valid IP address from transfer.info file\n");
	}
	if (!is_valid_port(server_port))
		throw std::runtime_error("\n not a valid port from transfer.info file\n");
	boost::asio::connect(socket, resolver.resolve(server_ip, server_port));

	std::cout << "Connected successfully to IP: " << server_ip << ", Port: " << server_port << std::endl;

	std::cout << "Client name for the request_Handler: " << this->client_name << "Length: " << this->client_name.length() << std::endl;
	std::cout << "Client id for request_handler: " << this->client_id << std::endl;

}

bool RequestsHandler::is_valid_ip(std::string ip) {
	return true; // update this function laterd

}
bool RequestsHandler::is_valid_port(std::string port) {
	return true;  // update this function later
}

void RequestsHandler::handle_registration() {
	std::cout << "Client id start of handle_registration: " << client_id << std::endl;
	for (uint8_t i = 0; i < NUM_OF_TRIALS; i++) {
		send_register_request();
		if (handle_register_response()) // Server responded with success and created a me.info file
			return;
	}
	std::string err = "Failed to register for " + std::to_string(NUM_OF_TRIALS) + " times\n";
	throw std::runtime_error(err);
}

void RequestsHandler::send_register_request() {

	std::unique_ptr<Payload> payload = std::make_unique<RegisterPayload>(this->client_name);
	//	RequestHeader(const std::string id, uint8_t version, uint16_t code, uint32_t payload_size);

	RequestHeader header = RequestHeader(this->client_id,this->client_version, REGISTER_REQUEST_CODE, payload->serialize().size());
		(this->client_id, this->client_version, REGISTER_REQUEST_CODE, payload->serialize().size());
	Packet packet(header, std::move(payload));

	std::vector<uint8_t> serialized_data = packet.serialize();

	std::cout << "Serialized Packet (hex): ";
	for (uint8_t byte : serialized_data) {
		printf("%02X ", byte);  // Print each byte in hex format for debugging purposes
	}
	std::cout << std::endl;

	boost::asio::write(this->socket, boost::asio::buffer(serialized_data, serialized_data.size()));
	std::cout << "Serialized data sent to server!\n";

}
bool RequestsHandler::handle_register_response() {

	ResponseHeader header = unpack_response_header();
	std::cout << "Received server version: " << static_cast<uint32_t>(header.get_server_version()) << ", Response code: " 
		<< header.get_response_code() << " Payload size: " << header.get_payload_size() << std::endl;

	if (header.get_response_code() == REGISTER_SUCCESS_CODE) {
		// receive the payload
		std::vector<uint8_t> buf(header.get_payload_size());
		boost::asio::read(this->socket, boost::asio::buffer(buf, header.get_payload_size()));
		this->client_id = std::string(buf.begin(), buf.end());
		std::cout << "Received client id: " << client_id << std::endl;
		write_me_info(this->client_name, this->client_id);
		return true;

	}
	return false; // registration attempt failed

}

ResponseHeader RequestsHandler:: unpack_response_header() {

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

	return ResponseHeader(server_version, response_code, payload_size);

}