#include "RequestsHandler.h"
#include "FileHandler.h"
#include "Endianness.h"
#include "cksum.h"


RequestsHandler::RequestsHandler(std::string cid, std::string cname, uint8_t cversion)
	: client_id(cid), resolver(io_context), socket(io_context), client_name(cname), client_version(cversion) {
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
	std::cout << "Client id for request_handler: " << this->client_id << std::endl;

}


std::optional<std::string> RequestsHandler::login_and_get_aes(std::string private_rsa_key) {
	for (int i = 0; i < NUM_OF_TRIALS; i++) {
		std::cout << "Trying to login for the " << i + 1 << " time\n";
		send_authenticate_request(LOGIN_REQUEST_CODE);
		std::optional<std::string> encrypted_aes = get_login_response();
		if (encrypted_aes) {
			std::cout << "Successfully login and got encrypted aes key from server! Decrypting...\n";
			RSAPrivateWrapper rsa_private(private_rsa_key);
			std::string decrypted_aes_key = rsa_private.decrypt(encrypted_aes.value());

			return decrypted_aes_key;  // Return the decrypted AES key

		}

	}
	std::cout << "Failed to login for " << NUM_OF_TRIALS << " Times." << std::endl;
	return std::nullopt;  // failed to login multiple times
}

std::optional<std::string>RequestsHandler::get_login_response() {
	ResponseHeader header = unpack_response_header();
	std::cout << "Received server version: " << static_cast<uint32_t>(header.get_server_version()) << ", Response code: "
		<< header.get_response_code() << " Payload size: " << header.get_payload_size() << std::endl;

	// receive the payload
	std::vector<uint8_t> buf(header.get_payload_size());
	boost::asio::read(this->socket, boost::asio::buffer(buf, header.get_payload_size()));
	std::cout << "Received client id: " << client_id << std::endl;
	if (header.get_response_code() == LOGIN_SUCCESS_CODE) {
		// take the key only
		std::string encrypted_aes_key = std::string(buf.begin() + ID_SIZE, buf.end());
		return encrypted_aes_key;
	}
	return std::nullopt;



}

std::string RequestsHandler::register_and_get_id() {
	// if register failed - return error (we can't proceed in program if both login and registration failed)

	for (uint8_t i = 0; i < NUM_OF_TRIALS; i++) {
		send_authenticate_request(REGISTER_REQUEST_CODE);
		std::optional<std::string> id = get_register_response_id();
		if (id) {
			std::cout << "Successfully received id: " << id.value() << std::endl;
			return id.value();
		}
	}
	std::string err = "Failed to register for " + std::to_string(NUM_OF_TRIALS) + " times\n";
	throw std::runtime_error(err);
}

std::optional<std::string> RequestsHandler::get_register_response_id() {
	// returns response if if response code was OK, else returns null

	ResponseHeader header = unpack_response_header();
	std::cout << "Received server version: " << static_cast<uint32_t>(header.get_server_version()) << ", Response code: "
		<< header.get_response_code() << " Payload size: " << header.get_payload_size() << std::endl;

	// receive the payload
	std::vector<uint8_t> buf(header.get_payload_size());
	boost::asio::read(this->socket, boost::asio::buffer(buf, header.get_payload_size()));
	std::cout << "Received client id: " << client_id << std::endl;
	if (header.get_response_code() == REGISTER_SUCCESS_CODE) {

		this->client_id = std::string(buf.begin(), buf.end());  // update client id 
		write_me_info(this->client_name, this->client_id);  // write into me.info the client name and id
		return this->client_id;
	}
	return std::nullopt;

}

void RequestsHandler::send_authenticate_request(uint16_t request_code) {

	std::unique_ptr<Payload> payload = std::make_unique<RegisterPayload>(this->client_name);
	//	RequestHeader(const std::string id, uint8_t version, uint16_t code, uint32_t payload_size);

	RequestHeader header = RequestHeader(this->client_id, this->client_version, request_code, payload->serialize().size());
	(this->client_id, this->client_version, request_code, payload->serialize().size());
	Packet packet(header, std::move(payload));

	std::vector<uint8_t> serialized_data = packet.serialize();

	std::cout << "Serialized Packet (hex): ";
	for (uint8_t byte : serialized_data) {
		printf("%02X ", byte);  // Print each byte in hex format for debugging purposes
	}
	std::cout << std::endl;

	boost::asio::write(this->socket, boost::asio::buffer(serialized_data, serialized_data.size()));
	std::cout << "authentication data sent to server!\n";

}



bool RequestsHandler::is_valid_ip(std::string ip) {
	return true; // update this function laterd

}
bool RequestsHandler::is_valid_port(std::string port) {
	return true;  // update this function later
}

void RequestsHandler::handle_send_file(std::string file_name, const std::string aes_key) {
	file_name.resize(FILE_NAME_SIZE, '\0'); // Resize and fill with null characters

	uint32_t orig_file_checksum = get_file_checksum(file_name);
	for (int i = 0; i < NUM_OF_TRIALS; i++) {
		send_file(file_name, aes_key);
		uint32_t server_checksum = get_send_file_response_crc();
		if (orig_file_checksum == server_checksum) {
			std::cout << "Both client and server generated same checksum: " << server_checksum << " Yay!" << std::endl;
			send_ack_after_crc(file_name, VALID_CRC_REQUEST_CODE);  // send "valid crc" and return
			get_ack_from_server();
			return;
		}
		else if (i != NUM_OF_TRIALS - 1) { // its not the last try
			std::cout << "Checksum is wrong. trying again..." << std::endl;
			send_ack_after_crc(file_name, INVALID_CRC_REQUEST_CODE); // send "invalid crc" and send file again (next loop)
			get_ack_from_server();
		}

	}
	std::string err = "Failed to receive correct checksum for " + std::to_string(NUM_OF_TRIALS) + " times\n";
	//throw std::runtime_error(err);
	std::cout << err;
	send_ack_after_crc(file_name, INVALID_CRC_FINAL_REQUEST_CODE);
	get_ack_from_server();
}
void RequestsHandler::get_ack_from_server() {
	ResponseHeader header = unpack_response_header();
	if (header.get_response_code() != SERVER_OK_CODE) {
		std::string err = "Expected server ok code: " + std::to_string(SERVER_OK_CODE) +
			", got instead: " + std::to_string(header.get_response_code()) + "\n";
		throw std::runtime_error(err);
	}

	std::vector<uint8_t> buf(header.get_payload_size());
	boost::asio::read(this->socket, boost::asio::buffer(buf, header.get_payload_size()));

	// we don't do anything with the client id





}
void RequestsHandler::send_ack_after_crc(const std::string file_name, uint16_t code) {
	std::unique_ptr<Payload> payload = std::make_unique<RegisterPayload>(file_name);
	//	RequestHeader(const std::string id, uint8_t version, uint16_t code, uint32_t payload_size);

	RequestHeader header = RequestHeader(this->client_id, this->client_version, code, payload->serialize().size());
	(this->client_id, this->client_version, code, payload->serialize().size());
	Packet packet(header, std::move(payload));

	std::vector<uint8_t> serialized_data = packet.serialize();

	boost::asio::write(this->socket, boost::asio::buffer(serialized_data, serialized_data.size()));
	std::cout << "Sent code: " << code << " To server " << std::endl;

}

uint32_t RequestsHandler::get_send_file_response_crc() {
	// function to receive the send file response
	// returns the checksum from the payload
	ResponseHeader header = unpack_response_header();
	if (header.get_response_code() != RECEIVED_FILE_SUCCESS_CODE)
		throw std::runtime_error("Server sent incorrect code, expected received file successfully code");

	// Payload: Client_id(16 bytes), content_size(4 bytes), file_name(255 bytes), checksum(4 bytes)
	// get last 4 bytes, that should be the cheecksum
	std::vector<uint8_t> buf(header.get_payload_size());
	boost::asio::read(this->socket, boost::asio::buffer(buf, header.get_payload_size()));

	uint32_t checksum = get_uint32_from_vec(buf, buf.size() - CHECKSUM_SIZE);
	std::cout << "Checksum received from server: " << checksum << std::endl;
	checksum = Endianness::from_little_to_native(checksum);
	std::cout << "Checksum after converting to native :" << checksum << std::endl;
	return checksum;

}

void RequestsHandler::send_file(std::string file_name, const std::string aes_key) {
	std::ifstream file(file_name, std::ios::binary);
	if (!file) {
		throw std::runtime_error("Error opening file: " + file_name);
	}

	// Get the file size
	file.seekg(0, std::ios::end);
	uint32_t orig_file_size = file.tellg();
	file.seekg(0, std::ios::beg);
	std::cout << "Original file size: " << orig_file_size << std::endl;
	// 1. fetch the data inside that file
		// 1. Fetch the data inside that file
	std::vector<char> file_data(orig_file_size);
	file.read(file_data.data(), orig_file_size);

	// Close the file after reading
	file.close();

	// Extend file name to 255 bytes
	//file_name.resize(FILE_NAME_SIZE, '\0'); // Resize and fill with null characters




	// 2. encrypt the data in that file
	AESWrapper aes(reinterpret_cast<const unsigned char*>(aes_key.c_str()), AESWrapper::DEFAULT_KEYLENGTH);
	std::string encrypted_data = aes.encrypt(file_data.data(), file_data.size());
	//std::cout << "Encrypted data from file: " << encrypted_data << std::endl;



	// 3. get the encrypted file size
	uint32_t encrypted_data_size = encrypted_data.size();
	std::cout << "Encrypted file size : " << encrypted_data_size << std::endl;
	// 4. create a SendFilePayload  payload
	uint16_t current_packet = 0;
	const uint16_t total_packets = (encrypted_data_size + BUFFER_SIZE - 1) / BUFFER_SIZE;
	// 5. create a header and packet,

	for (uint16_t packet_num = 1; packet_num <= total_packets; ++packet_num) {
		// Calculate the offset and the size of the current chunk
		uint32_t offset = (packet_num - 1) * BUFFER_SIZE;
		uint32_t chunk_size = std::min(static_cast<uint32_t>(BUFFER_SIZE), encrypted_data_size - offset);

		// Extract the chunk from the encrypted data
		std::string chunk_data = encrypted_data.substr(offset, chunk_size);

		// 5. Create a SendFilePayload payload for the current chunk
		std::unique_ptr<Payload> payload = std::make_unique<SendFilePayload>
			(encrypted_data_size, orig_file_size, packet_num, total_packets, file_name, chunk_data);


		RequestHeader header = RequestHeader(this->client_id, this->client_version, SEND_FILE_REQUEST_CODE, payload->serialize().size());
		(this->client_id, this->client_version, REGISTER_REQUEST_CODE, payload->serialize().size());
		Packet packet(header, std::move(payload));

		std::vector<uint8_t> serialized_data = packet.serialize();


		// 7. Send the packet over the network
		boost::asio::write(this->socket, boost::asio::buffer(serialized_data, serialized_data.size()));
		std::cout << "Serialized  encrypted file data sent to server, total size: " << serialized_data.size() << std::endl;
	}

}
std::string RequestsHandler::get_encrypted_aes(const std::string private_rsa_key) {
	ResponseHeader header = unpack_response_header();
	if (not (header.get_response_code() == RECEIVED_KEY_SUCCESS_CODE or header.get_response_code() == LOGIN_SUCCESS_CODE))
		throw std::runtime_error("Server failed to send encrypted key back. not good\n");

	// Error returned encrypted aes key
	std::cout << "Server sent key success code. now trying to get the encrypted key\n";
	std::vector<uint8_t> buf(header.get_payload_size());
	boost::asio::read(this->socket, boost::asio::buffer(buf, header.get_payload_size()));
	std::string client_id = std::string(buf.begin(), buf.begin() + ID_SIZE);
	std::string encrypted_key = std::string(buf.begin() + ID_SIZE, buf.end());
	//std::cout << "Encrypted key received: " << encrypted_key << std::endl;


	// Decrypt the AES key using the client's private RSA key
	RSAPrivateWrapper rsa_private(private_rsa_key);
	std::string decrypted_aes_key = rsa_private.decrypt(encrypted_key);

	//std::cout << "Decrypted AES key: " << decrypted_aes_key << std::endl;

	return decrypted_aes_key;  // Return the decrypted AES key

}

void RequestsHandler::send_public_key(const std::string public_key) {
	// sends the public key given by the client

	RSAPublicWrapper pub(public_key);

	std::unique_ptr<Payload> payload = std::make_unique<SendKeyPayload>(this->client_name, public_key);

	RequestHeader header = RequestHeader(this->client_id, this->client_version, SEND_KEY_REQUEST_CODE, payload->serialize().size());
	(this->client_id, this->client_version, REGISTER_REQUEST_CODE, payload->serialize().size());
	Packet packet(header, std::move(payload));

	std::vector<uint8_t> serialized_data = packet.serialize();

	std::cout << "Serialized Packet (hex): ";
	for (uint8_t byte : serialized_data) {
		printf("%02X ", byte);  // Print each byte in hex format for debugging purposes
	}
	std::cout << std::endl;

	boost::asio::write(this->socket, boost::asio::buffer(serialized_data, serialized_data.size()));
	std::cout << "Serialized public key sent to server!\n";


}




ResponseHeader RequestsHandler::unpack_response_header() {

	// fill up the response parts : version, code, payload size.
	std::vector<uint8_t> response_buffer(SERVER_RESPONSE_HEADER_SIZE);
	boost::asio::read(this->socket, boost::asio::buffer(response_buffer, response_buffer.size()));

	// Extract server version (1 byte), response code (2 bytes), and payload size (4 bytes)

	uint8_t server_version = response_buffer[0];

	uint16_t response_code = response_buffer[1] | response_buffer[2] << 8;

	uint32_t payload_size = get_uint32_from_vec(response_buffer, 3);

	// The sizes are in little endian, convert to native
	response_code = Endianness::from_little_to_native(response_code);
	payload_size = Endianness::from_little_to_native(payload_size);

	return ResponseHeader(server_version, response_code, payload_size);

}

uint32_t RequestsHandler::get_uint32_from_vec(const std::vector<uint8_t>& vec, uint32_t i) {
	// returns the uint32_t sitting at index i, i+1, i+2, i+3
	uint32_t res = vec[i] | vec[i + 1] << 8
		| vec[i + 2] << 16 | vec[i + 3] << 24;
	return res;

}
uint16_t RequestsHandler::get_uint16_from_vec(const std::vector<uint8_t>& vec, uint32_t i) {
	// returns the uint16_t sitting at index i, i+1

	uint16_t res = vec[i] | vec[i + 1] << 8;
	return res;
}
void RequestsHandler::set_id(std::string id) {
	this->client_id = id;
}
void RequestsHandler::close_connection() {
	if (this->socket.is_open()) {
		boost::system::error_code ec;

		// First, shut down the socket properly
		socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
		if (ec && ec != boost::asio::error::not_connected) {
			// Log the error but continue with cleanup
			std::cerr << "Socket shutdown error: " << ec.message() << std::endl;
		}

		// Then close the socket
		socket.close(ec);
		if (ec) {
			std::cerr << "Socket close error: " << ec.message() << std::endl;
		}
		else {
			std::cout << "Socket closed successfully\n";
		}
	}
}