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
	check_if_registered();

	// Initialize client_name to a size of 255
	this->client_name.resize(CLIENT_NAME_SIZE, '\0'); // Resize and fill with null characters
	std::cout << "Client initialized , along with requests handler." << std::endl;
	std::cout << "Client name:" << client_name << ", length: " << this->client_name.length() << 
		"file path:" << file_path << " registered: " <<registered << std::endl;
	if (this->registered)
		std::cout << "Client is already registered." << std::endl;


}
void Client::check_if_registered() {
	// try to open me.info file, if this exists, then this->registed = true, and load keys


}
void Client::start() {
// send registeration request -> receive msg, send key, receive key, send file encrypted, receive OK
	if (!this->registered) {
		this->requests_handler.handle_registration();
		//this->requests_handler.send_register_request();
		//this->requests_handler.receive_register_response();
		//this->requests_handler.send_key();
		//this->requests_handler.receive_key();
	}
	else {
		//this->requests_handler.send_login_request();
		//this->requests_handler.receive_login_response();

	}
	//this->requests_handler.send_encrypted_file();

}




	int main2() {
		// test_messages();
		//test_sending();





		return 0;
	}
