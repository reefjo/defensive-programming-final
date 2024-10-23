#include "RequestsHandler.h"
#include "FileHandler.h"
#include "Endianness.h"


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

bool RequestsHandler::is_valid_ip(std::string ip) {
	return true; // update this function laterd

}
bool RequestsHandler::is_valid_port(std::string port) {
	return true;  // update this function later
}
void RequestsHandler::send_file(std::string file_name,  const std::string aes_key) {
	std::ifstream file(file_name, std::ios::binary);
	if (!file) {
		std::cerr << "Error opening file: " << file_name << std::endl;
		return;
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
	file_name.resize(FILE_NAME_SIZE, '\0'); // Resize and fill with null characters




	// 2. encrypt the data in that file
	AESWrapper aes(reinterpret_cast<const unsigned char*>(aes_key.c_str()), AESWrapper::DEFAULT_KEYLENGTH);
	std::string encrypted_data = aes.encrypt(file_data.data(), file_data.size());
	std::cout << "Encrypted data from file: " << encrypted_data << std::endl;



	// 3. get the encrypted file size
	uint32_t encrypted_data_size = encrypted_data.size();
	std::cout << "Encrypted file size : " << encrypted_data_size << std::endl;
	// 4. create a SendFilePayload  payload
	uint16_t current_packet = 0;
	const uint16_t total_packets = (encrypted_data_size + BUFFER_SIZE - 1) / BUFFER_SIZE;
	// 5. create a header and packet,

	for (uint16_t packet_num = 1; packet_num <= total_packets; ++packet_num) {
		// Calculate the offset and the size of the current chunk
		uint32_t offset = (packet_num - 1)* BUFFER_SIZE;
		uint32_t chunk_size = std::min(static_cast<uint32_t>(BUFFER_SIZE), encrypted_data_size - offset);

		// Extract the chunk from the encrypted data
		std::string chunk_data = encrypted_data.substr(offset, chunk_size);

		// 5. Create a SendFilePayload payload for the current chunk
		std::unique_ptr<Payload> payload = std::make_unique<SendFilePayload>
			(encrypted_data_size, orig_file_size, packet_num, total_packets,file_name, chunk_data);


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
	std::string client_id  = std::string(buf.begin(), buf.begin() + ID_SIZE);
	std::string encrypted_key = std::string(buf.begin() + ID_SIZE, buf.end());
	std::cout << "Encrypted key received: " << encrypted_key << std::endl;


		// Decrypt the AES key using the client's private RSA key
	RSAPrivateWrapper rsa_private(private_rsa_key);
	std::string decrypted_aes_key = rsa_private.decrypt(encrypted_key);

	std::cout << "Decrypted AES key: " << decrypted_aes_key << std::endl;

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