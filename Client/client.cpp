#include <iostream>
#include <fstream> 
#include <string>    // for std::string, std::getline, std::stoi
#include <boost/asio.hpp>
#include "FileHandler.h"
#include "Client.h"


// Public constructor: Initializes the client by reading server details and the client's info from transfer file.

Client::Client()
	: Client(read_transfer_file(), DEFAULT_ID) {}  // Delegates to private constructor



// Private constructor: Initializes the Client object with server details and prepares the RequestsHandler.
// Parameters: 
// - t: Tuple containing server info (IP, port, client name, file path).
// - client_id: The client's unique identifier (loaded from file or generated).
Client::Client(std::tuple<std::string, std::string, std::string, std::string> t, std::string client_id)
	: client_name(std::get<2>(t)),
	file_path(std::get<3>(t)),
	client_id(client_id),
	requests_handler(client_id, client_name, CLIENT_VERSION) {

	// Initialize client_name to a size of 255
	this->client_name.resize(CLIENT_NAME_SIZE, '\0'); // Resize and fill with null characters (regardless if registered)

}


// Converts a hexadecimal string to a binary string.
// Each 2 hex characters represent 1 byte.
std::string Client::hex_to_binary(const std::string& hex_str) {
	// should be in FileHandler, if i have time - i will move it
	std::string binary;
	binary.reserve(hex_str.length() / 2);  // Each 2 hex chars represents 1 byte

	for (std::size_t i = 0; i < hex_str.length(); i += 2) {
		std::string byte_string = hex_str.substr(i, 2);  // Get two hex chars
		char byte = static_cast<char>(std::stoi(byte_string, nullptr, 16));  // Convert to byte
		binary.push_back(byte);  // Append to result
	}

	return binary;
}


// Loads stored credentials from a file (me.info).
// If the file is found, it retrieves the client's name, ID, and private key.
// If no file is found or the contents are invalid, it skips the loading process.
void Client::load_stored_credentials() {

	// try to open me.info file and try to fetch the stored information
	std::vector<std::string> res;
	std::ifstream file(ME_INFO_FILE_NAME);

	if (!file) {
		return;  // File doesn't exist
	}

	// Read first two lines (name and id)
	std::string line;
	for (int i = 0; i < 2 && std::getline(file, line); i++) {
		res.push_back(line);
	}

	// Make sure we have at least the first two lines
	if (res.size() < 2) {
		file.close();
		return;
	}


	// Read the rest of the file as the key
	std::stringstream key_stream;
	key_stream << file.rdbuf();  // read all remaining content
	std::string encoded_key = key_stream.str();

	file.close();
	this->registered = true;
	this->client_name = res[0];
	std::string hex_client_id = res[1];
	std::string client_id_binary = hex_to_binary(hex_client_id);
	this->client_id = client_id_binary;
	this->requests_handler.set_id(client_id_binary);

	if (!encoded_key.empty()) {
		// Found a key, fetch it
		try {
			std::cout << "Found key in me.info, attempting to decode...\n";
			this->private_rsa_key = Base64Wrapper::decode(encoded_key);
		}
		catch (const std::exception& e) {
			std::cerr << "Failed to decode key: " << e.what() << std::endl;
		}
	}
}


// Attempts to register the client with the server.
// If successful, it generates keys, sends the public key to the server, and retrieves the AES key.
// Returns true if registration succeeds, false otherwise.
bool Client::attempt_register() {
	try {
		std::optional<std::string> id = this->requests_handler.register_and_get_id();
		if (not id)
			return false;
		this->client_id = id.value();
		this->requests_handler.set_id(id.value());
		std::string public_key = this->generate_keys();
		std::string base64key = Base64Wrapper::encode(this->private_rsa_key);
		put_key_in_files(base64key);
		this->requests_handler.send_public_key(public_key);
		this->aes_key = this->requests_handler.get_encrypted_aes(this->private_rsa_key);
		return true;  // Registration successful
	}
	catch (const std::exception& e) {
		std::cerr << "Registration failed: " << e.what() << std::endl;
		return false;  // Registration failed
	}
}


// Attempts to log in using stored credentials.
// If login succeeds, retrieves the AES key from the server.
// Returns true if login succeeds, false otherwise.
bool Client::attempt_login() {
	if (not this->registered)
		return false; // no reason to attempt login if user not registered
	std::optional<std::string> res = this->requests_handler.login_and_get_aes(this->private_rsa_key);
	if (res) { // login succeed, got aes key
		this->aes_key = res.value();
		return true;
	}
	return false;

}


// Generates an RSA key pair and stores the private key internally.
// Returns the public key for sending to the server.
std::string Client::generate_keys() {
	// Generates public&priavte keys. stores private key in object and returns public key
	RSAPrivateWrapper rsa_private;
	this->private_rsa_key = rsa_private.getPrivateKey();
	return rsa_private.getPublicKey();  // Return public key for sending to server
}


// Starts the client workflow: loads credentials, attempts login, and handles file transfer.
// If login fails, it attempts to register the client.
// Sends the encrypted file to the server and then closes the connection.
void Client::start() {
	// send registeration request -> receive msg, send key, receive key, send file encrypted, receive OK
	load_stored_credentials();
	std::cout << "Client id after loading: " << this->client_id << std::endl;
	if (not attempt_login()) {
		std::cout << "Login failed, attempting to register...\n";
		if (not attempt_register())
			throw std::runtime_error("Failed to login and register. can't proceed\n");
	}

	this->requests_handler.handle_send_file(this->file_path, this->aes_key);
	this->requests_handler.close_connection();

}


