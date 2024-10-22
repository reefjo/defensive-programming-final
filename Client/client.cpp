#include <iostream>
#include <fstream> 
#include <string>    // for std::string, std::getline, std::stoi
#include <boost/asio.hpp>
#include "FileHandler.h"
#include "Client.h"




/*
void test_sending() {
	try {
		boost::asio::io_service io_service;

		// Create socket and connect to server
		tcp::socket socket(io_service);
		FileHandler fileHandler;

		// Read connection info from transfer file
		auto [server_ip, server_port, client_name, file_path] = fileHandler.readTransferFile();

		// Resolve server address and port
		tcp::resolver resolver(io_service);
		auto endpoint = resolver.resolve({ server_ip, server_port });
		boost::asio::connect(socket, endpoint);

		std::cout << "Connected to server!" << std::endl;

		// Example: send and receive file
		std::string fileToSend = "client_to_send" + std::string(".txt");  // Modify with your file path
		std::string fileToReceive = "client_to_receive.txt";  // Modify with your desired file path

		// Send file to server
		fileHandler.sendFile(fileToSend, socket);

		// Receive file from server
		//fileHandler.receiveFile(fileToReceive, socket);

		// Close socket
		socket.close();
	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}

}
*/

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

	//check_if_registered();

	std::cout << "Client initialized , along with requests handler." << std::endl;
	std::cout << "Client name for client:" << client_name << ", length: " << this->client_name.length() << 
		"file path:" << file_path << " registered: " <<registered << std::endl;
	std::cout << "Client id end of constructoer: " << this->client_id << std::endl;

	/*
	if (this->registered)
		std::cout << "Client is already registered." << std::endl; 
		*/


}
void Client::check_if_registered() {
	
	// try to open me.info file, if this exists, then this->registed = true, and load keys
	std::vector<std::string> res = read_key_from_file();
	if (res.empty())
		return;
	// result will contain either 2 elements: cname, id or 3 elements: cname, id, key
	this->registered = true;
	this->client_name = res[0];
	this->client_id = res[1];
	if (res.size() == 3) {
		this->aes_key = res[2];
		this->sent_public_key = true;
	}
	


}

std::string Client::generate_keys() {
	RSAPrivateWrapper rsa_private;
	this->private_rsa_key = rsa_private.getPrivateKey();  // Store private key for later use
	return rsa_private.getPublicKey();  // Return public key for sending to server
}

void Client::start() {
// send registeration request -> receive msg, send key, receive key, send file encrypted, receive OK

	if (!this->registered) {
		this->requests_handler.handle_registration();
		std::string public_key = this->generate_keys();
		std::string base64key = Base64Wrapper::encode(this->private_rsa_key);
		put_key_in_files(base64key);
		this->requests_handler.send_public_key(public_key);
	}
	
	else {
		std::cout << "Client alredy registered . (msg from client start). proceeding to login.\n";
		//this->requests_handler.handle_login();
	}
	
	this->aes_key = this->requests_handler.get_encrypted_aes(this->private_rsa_key);
	//this->requests_handler.send_file();


}




	int main2() {
		// test_messages();
		//test_sending();





		return 0;
	}
