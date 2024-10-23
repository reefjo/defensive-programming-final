#include <iostream>
#include <fstream> 
#include <string>    // for std::string, std::getline, std::stoi
#include <boost/asio.hpp>
#include "FileHandler.h"
#include "Client.h"






// Public constructor: handles file reading and then delegates to the private constructor
Client::Client()
	: Client(read_transfer_file(), DEFAULT_ID) {}  // Delegates to private constructor

// Private constructor: fully initializes the requests_handler after reading the file

Client::Client(std::tuple<std::string, std::string, std::string, std::string> t, std::string client_id )
	: client_name(std::get<2>(t)),
	file_path(std::get<3>(t)),
	client_id(client_id),
	requests_handler(client_id, client_name, CLIENT_VERSION) {

	std::cout << "Client constructor, before creating RequestsHandler. client_id: '" << client_id
		<< "', client_name: '" << client_name << "'" << std::endl;



	// Initialize client_name to a size of 255
	this->client_name.resize(CLIENT_NAME_SIZE, '\0'); // Resize and fill with null characters (regardless if registered)

	//load_stored_credentials();

	std::cout << "Client initialized , along with requests handler." << std::endl;
	std::cout << "Client name for client:" << client_name << ", length: " << this->client_name.length() << 
		"file path:" << file_path << " registered: " <<registered << std::endl;
	std::cout << "Client id end of constructoer: " << this->client_id << std::endl;

	/*
	if (this->registered)
		std::cout << "Client is already registered." << std::endl; 
		*/


}

std::string Client:: hex_to_binary(const std::string& hex_str) {
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
	key_stream << file.rdbuf();  // Read all remaining content
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




std::string Client::generate_keys(){
	// Generates public&priavte keys. stores private key in object and returns public key
	RSAPrivateWrapper rsa_private;
	this->private_rsa_key = rsa_private.getPrivateKey();  // Store private key for later use
	return rsa_private.getPublicKey();  // Return public key for sending to server
}

void Client::start() {
// send registeration request -> receive msg, send key, receive key, send file encrypted, receive OK
	load_stored_credentials();
	if (not attempt_login()) {
		std::cout << "Login failed, attempting to register...\n";
		if (not attempt_register())
			throw std::runtime_error("Failed to login and register. can't proceed\n");
	}
	 
	this->requests_handler.handle_send_file(TEST_FILE_NAME, this->aes_key);

}
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
bool Client::attempt_login() {
	if (not this->registered)
		return false; // no reason to attempt login if user never registered..
	std::optional<std::string> res = this->requests_handler.login_and_get_aes(this->private_rsa_key);
	if (res) { // login succeed, got aes key
		this->aes_key = res.value();
		return true;
	}
	return false;
	


}





	int main2() {
		// test_messages();
		//test_sending();





		return 0;
	}
