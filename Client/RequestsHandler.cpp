#include "RequestsHandler.h"


// Initializes the RequestHandler with client data and connects to the server.
RequestsHandler::RequestsHandler(std::string cid, std::string cname, uint8_t cversion)
	: client_id(cid), resolver(io_context), socket(io_context), client_name(cname), client_version(cversion) {

	// Ensure the client name is exactly CLIENT_NAME_SIZE characters, padded with null characters if necessary.
	this->client_name.resize(CLIENT_NAME_SIZE, '\0');

	// Read server IP and port from the transfer file.
	auto [server_ip, server_port, _, file_path] = read_transfer_file();

	// Establish a connection to the server.
	boost::asio::connect(socket, resolver.resolve(server_ip, server_port));

	std::cout << "Connected successfully to IP: " << server_ip << ", Port: " << server_port << std::endl;


}

// Attempts to log in to the server and retrieve the encrypted AES key.
std::optional<std::string> RequestsHandler::login_and_get_aes(std::string private_rsa_key) {
	for (int i = 0; i < NUM_OF_TRIALS; i++) {
		std::cout << "Trying to login for the " << i + 1 << " time\n";
		send_authenticate_request(LOGIN_REQUEST_CODE);

		// Try to retrieve the login response (encrypted AES key)
		std::optional<std::string> encrypted_aes = get_login_response();

		if (encrypted_aes) {
			std::cout << "Successfully login and got encrypted aes key from server! Decrypting...\n";

			// Decrypt the AES key using the RSA private key
			RSAPrivateWrapper rsa_private(private_rsa_key);
			std::string decrypted_aes_key = rsa_private.decrypt(encrypted_aes.value());

			return decrypted_aes_key;  // Return the decrypted AES key

		}

	}
	std::cout << "Failed to login for " << std::to_string(NUM_OF_TRIALS) << " Times." << std::endl;
	return std::nullopt;  // failed to login multiple times
}


// Unpacks the login response from the server, including the server's response code and payload (AES key) if login was successful.
std::optional<std::string>RequestsHandler::get_login_response() {
	// Get the response header from the server
	ResponseHeader header = unpack_response_header();

	std::cout << "Received server version: " << static_cast<uint32_t>(header.get_server_version()) << ", Response code: "
		<< header.get_response_code() << " Payload size: " << header.get_payload_size() << std::endl;

	// Allocate a buffer and receive the payload (AES key)
	std::vector<uint8_t> buf(header.get_payload_size());
	boost::asio::read(this->socket, boost::asio::buffer(buf, header.get_payload_size()));

	if (header.get_response_code() == LOGIN_SUCCESS_CODE) {
		// Extract the AES key from the response payload
		std::string encrypted_aes_key = std::string(buf.begin() + ID_SIZE, buf.end());
		return encrypted_aes_key;
	}
	return std::nullopt;  // login was not successful

}

// Registers the client with the server and retrieves the client ID.
// if registration fails - throws error 

std::string RequestsHandler::register_and_get_id() {

	for (uint8_t i = 0; i < NUM_OF_TRIALS; i++) {
		send_authenticate_request(REGISTER_REQUEST_CODE);
		std::optional<std::string> id = get_register_response_id();
		if (id) {
			std::cout << "Successfully received id from server " << std::endl;
			return id.value();
		}
	}
	std::string err = "Failed to register for " + std::to_string(NUM_OF_TRIALS) + " times\n";
	throw std::runtime_error(err);
}

// Retrieves the registration response (client ID) from the server if successful, otherwise returns nullopt
std::optional<std::string> RequestsHandler::get_register_response_id() {


	ResponseHeader header = unpack_response_header();
	std::cout << "Received server version: " << static_cast<uint32_t>(header.get_server_version()) << ", Response code: "
		<< header.get_response_code() << " Payload size: " << header.get_payload_size() << std::endl;

	// receive the payload
	std::vector<uint8_t> buf(header.get_payload_size());
	boost::asio::read(this->socket, boost::asio::buffer(buf, header.get_payload_size()));

	if (header.get_response_code() == REGISTER_SUCCESS_CODE) {

		this->client_id = std::string(buf.begin(), buf.end());  // update client id 
		write_me_info(this->client_name, this->client_id);  // write into me.info the client name and id
		return this->client_id;
	}
	return std::nullopt;

}

// Authenticates the client by sending either a login or registration request.
void RequestsHandler::send_authenticate_request(uint16_t request_code) {
	// Create a payload containing the client name
	std::unique_ptr<Payload> payload = std::make_unique<RegisterPayload>(this->client_name);

	RequestHeader header = RequestHeader(this->client_id, this->client_version, request_code, payload->serialize().size());
	Packet packet(header, std::move(payload));

	std::vector<uint8_t> serialized_data = packet.serialize();

	// Send the serialized data to the server
	boost::asio::write(this->socket, boost::asio::buffer(serialized_data, serialized_data.size()));
	std::cout << "authentication data sent to server!\n";
}

// Handles the process of sending a file in chunks to the server, performs checksum validation in the end.
void RequestsHandler::handle_send_file(std::string file_name, const std::string aes_key) {

	// Resize and pad the file name to a fixed size with null characters
	file_name.resize(FILE_NAME_SIZE, '\0'); // Resize and fill with null characters

	// Get the original file checksum (CRC32)
	uint32_t orig_file_checksum = get_file_checksum(file_name);

	// Attempt to send the file and verify checksum, retrying up to NUM_OF_TRIALS times
	for (int i = 0; i < NUM_OF_TRIALS; i++) {
		send_file(file_name, aes_key);
		uint32_t server_checksum = get_send_file_response_crc();

		// If the checksums match, send acknowledgment, get acknowledgment, and return
		if (orig_file_checksum == server_checksum) {
			std::cout << "Both client and server generated same checksum: " << server_checksum << " Yay!" << std::endl;
			send_ack_after_crc(file_name, VALID_CRC_REQUEST_CODE);  // send "valid crc" and return
			get_ack_from_server();
			return;
		}
		else if (i != NUM_OF_TRIALS - 1) { // Retry if this is not the last attempt
			std::cout << "Checksum is wrong. trying again..." << std::endl;
			send_ack_after_crc(file_name, INVALID_CRC_REQUEST_CODE); // send "invalid crc" and send file again (next loop)
			get_ack_from_server();
		}

	}

	// If checksum validation failed after multiple attempts, send final invalid CRC acknowledgment
	std::string err = "Failed to receive correct checksum for " + std::to_string(NUM_OF_TRIALS) + " times\n";
	//throw std::runtime_error(err);
	std::cout << err;
	send_ack_after_crc(file_name, INVALID_CRC_FINAL_REQUEST_CODE);
	get_ack_from_server();
}

// Retrieves acknowledgment from the server after sending the file.
void RequestsHandler::get_ack_from_server() {
	// Unpack the response header from the server
	ResponseHeader header = unpack_response_header();

	if (header.get_response_code() != SERVER_OK_CODE) {
		std::string err = "Expected server ok code: " + std::to_string(SERVER_OK_CODE) +
			", got instead: " + std::to_string(header.get_response_code()) + "\n";
		throw std::runtime_error(err);
	}

	// Read the payload (if any) but do nothing with the data because its just acknowledgment
	std::vector<uint8_t> buf(header.get_payload_size());
	boost::asio::read(this->socket, boost::asio::buffer(buf, header.get_payload_size()));

}

// Sends acknowledgment to the server after checksum validation.
void RequestsHandler::send_ack_after_crc(const std::string file_name, uint16_t code) {
	// Create a payload with the file name
	std::unique_ptr<Payload> payload = std::make_unique<RegisterPayload>(file_name);

	// Prepare a request header for sending acknowledgment
	RequestHeader header = RequestHeader(this->client_id, this->client_version, code, payload->serialize().size());

	Packet packet(header, std::move(payload));

	std::vector<uint8_t> serialized_data = packet.serialize();

	// Send the serialized acknowledgment to the server
	boost::asio::write(this->socket, boost::asio::buffer(serialized_data, serialized_data.size()));
	std::cout << "Sent code: " << code << " To server " << std::endl;

}

// Retrieves the server's  checksum after sending the file.
uint32_t RequestsHandler::get_send_file_response_crc() {
	// Unpack the response header from the server
	ResponseHeader header = unpack_response_header();

	// Verify that the response code matches the expected CRC response code
	if (header.get_response_code() != RECEIVED_FILE_SUCCESS_CODE) {
		throw std::runtime_error("expected successful file received code, server sent other code ");
	}

	// Allocate a buffer, receive the payload, fetch only the checksum
	std::vector<uint8_t> buf(header.get_payload_size());
	boost::asio::read(this->socket, boost::asio::buffer(buf, header.get_payload_size()));

	uint32_t checksum = get_uint32_from_vec(buf, buf.size() - CHECKSUM_SIZE);  // fetch checksum from the last 4 bytes using helper func
	checksum = Endianness::from_little_to_native(checksum);
	std::cout << "Checksum after converting to native :" << checksum << std::endl;
	return checksum;
}


// Sends an encrypted file to the server in chunks, using the AES key for encryption.
// The file is read from the local filesystem, encrypted, and sent over the network in chunks.
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

	// Read the entire file into a buffer
	std::vector<char> file_data(orig_file_size);
	file.read(file_data.data(), orig_file_size);
	file.close();


	// Encrypt the file data using AES with the provided key
	AESWrapper aes(reinterpret_cast<const unsigned char*>(aes_key.c_str()), AESWrapper::DEFAULT_KEYLENGTH);
	std::string encrypted_data = aes.encrypt(file_data.data(), file_data.size());



	// Get the size of the encrypted file data
	uint32_t encrypted_data_size = encrypted_data.size();
	std::cout << "Encrypted file size : " << encrypted_data_size << std::endl;

	// Determine how many packets are needed to send the file data
	uint16_t current_packet = 0;
	const uint16_t total_packets = (encrypted_data_size + BUFFER_SIZE - 1) / BUFFER_SIZE;

	// Loop through the file and send it in chunks (packets)
	for (uint16_t packet_num = 1; packet_num <= total_packets; ++packet_num) {
		// Calculate the offset and the size of the current chunk
		uint32_t offset = (packet_num - 1) * BUFFER_SIZE;
		uint32_t chunk_size = std::min(static_cast<uint32_t>(BUFFER_SIZE), encrypted_data_size - offset);

		// Extract the chunk from the encrypted data
		std::string chunk_data = encrypted_data.substr(offset, chunk_size);

		// 5. Create a SendFilePayload payload for the current chunk
		std::unique_ptr<Payload> payload = std::make_unique<SendFilePayload>
			(encrypted_data_size, orig_file_size, packet_num, total_packets, file_name, chunk_data);

		// Create a header for the current packet and serialize it
		RequestHeader header = RequestHeader(this->client_id, this->client_version, SEND_FILE_REQUEST_CODE, payload->serialize().size());
		Packet packet(header, std::move(payload));

		// Serialize the packet and send it over the network
		std::vector<uint8_t> serialized_data = packet.serialize();
		boost::asio::write(this->socket, boost::asio::buffer(serialized_data, serialized_data.size()));
		std::cout << "Serialized  encrypted file data sent to server, total size: " << serialized_data.size() << std::endl;
	}
}


// Retrieves the AES key encrypted by the server using the client's RSA public key.
// The AES key is then decrypted using the client's RSA private key.
std::string RequestsHandler::get_encrypted_aes(const std::string private_rsa_key) {
	// Unpack the response header from the server
	ResponseHeader header = unpack_response_header();

	if (header.get_response_code() != RECEIVED_KEY_SUCCESS_CODE) {
		throw std::runtime_error("Server failed to send encrypted key back.\n");
	}

	std::cout << "Server sent key success code. now trying to get the encrypted key\n";

	// Read response into the buffer
	std::vector<uint8_t> buf(header.get_payload_size());
	boost::asio::read(this->socket, boost::asio::buffer(buf, header.get_payload_size()));

	// Extract the client ID and the encrypted AES key from the response
	std::string client_id = std::string(buf.begin(), buf.begin() + ID_SIZE);
	std::string encrypted_key = std::string(buf.begin() + ID_SIZE, buf.end());

	// Decrypt the AES key using the client's private RSA key
	RSAPrivateWrapper rsa_private(private_rsa_key);
	std::string decrypted_aes_key = rsa_private.decrypt(encrypted_key);

	std::cout << "Successfuly received the encrypted key from server, and decrypted it.\n";

	return decrypted_aes_key;  // Return the decrypted AES key

}


// Sends the client's public RSA key to the server (The server uses this key to encrypt future communications)
void RequestsHandler::send_public_key(const std::string public_key) {
	// sends the public key given by the client

	RSAPublicWrapper pub(public_key);

	// Create a packet containing the client's name and public key
	std::unique_ptr<Payload> payload = std::make_unique<SendKeyPayload>(this->client_name, public_key);
	RequestHeader header = RequestHeader(this->client_id, this->client_version, SEND_KEY_REQUEST_CODE, payload->serialize().size());
	Packet packet(header, std::move(payload));

	// Serialize the packet for sending
	std::vector<uint8_t> serialized_data = packet.serialize();

	// Send the serialized public key to the server
	boost::asio::write(this->socket, boost::asio::buffer(serialized_data, serialized_data.size()));
	std::cout << "Serialized public key sent to server!\n";
}

// Unpacks a response header sent by the server.
// Extracts and returns the server version, response code, and payload size.
ResponseHeader RequestsHandler::unpack_response_header() {

	// fill up the response parts : version, code, payload size.
	std::vector<uint8_t> response_buffer(SERVER_RESPONSE_HEADER_SIZE);
	boost::asio::read(this->socket, boost::asio::buffer(response_buffer, response_buffer.size()));

	// Extract server version (1 byte), response code (2 bytes), and payload size (4 bytes)

	uint8_t server_version = response_buffer[0];

	uint16_t response_code = get_uint16_from_vec(response_buffer, 1); // get the uint_16t sitting at indices i, i + 1

	uint32_t payload_size = get_uint32_from_vec(response_buffer, 3); // get the uint_32t sitting at indices i to i + 3

	// The sizes are in little endian, convert to native
	response_code = Endianness::from_little_to_native(response_code);
	payload_size = Endianness::from_little_to_native(payload_size);

	// Return the response header with extracted values
	return ResponseHeader(server_version, response_code, payload_size);
}


// Extracts a uint32_t from the given vector starting at the specified index.
// This is used for parsing multi-byte values from a byte vector.

uint32_t RequestsHandler::get_uint32_from_vec(const std::vector<uint8_t>& vec, uint32_t i) {
	// returns the uint32_t sitting at index i, i+1, i+2, i+3
	uint32_t res = vec[i] | vec[i + 1] << 8
		| vec[i + 2] << 16 | vec[i + 3] << 24;
	return res;

}

// Extracts a uint16_t from the given vector starting at the specified index.
// This is used for parsing 2-byte values from a byte vector.
uint16_t RequestsHandler::get_uint16_from_vec(const std::vector<uint8_t>& vec, uint32_t i) {
	// returns the uint16_t sitting at index i, i+1

	uint16_t res = vec[i] | vec[i + 1] << 8;
	return res;
}

// Sets the client ID for the request handler.
void RequestsHandler::set_id(std::string id) {
	this->client_id = id;
}


// Shuts down the connection and closes the socket.
void RequestsHandler::close_connection() {
	if (this->socket.is_open()) {
		boost::system::error_code ec;

		// Shut down the socket 
		socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
		if (ec && ec != boost::asio::error::not_connected) {
			// Log the error but continue with cleanup
			std::cerr << "Socket shutdown error: " << ec.message() << std::endl;
		}

		// Close the socket
		socket.close(ec);
		if (ec) {
			std::cerr << "Socket close error: " << ec.message() << std::endl;
		}
		else {
			std::cout << "Socket closed successfully\n";
		}
	}
}