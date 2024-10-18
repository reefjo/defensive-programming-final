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
	: Client(read_transfer_file()) {}  // Delegates to private constructor

// Private constructor: fully initializes the requests_handler after reading the file

Client::Client(std::tuple<std::string, std::string, std::string, std::string> t)
	: client_name(std::get<2>(t)),
	file_path(std::get<3>(t)),
	requests_handler(client_name, CLIENT_VERSION){
	check_if_registered();
	// Now the client is fully initialized, including requests_handler
	std::cout << "Client initialized , along with requests handler." << std::endl;
	if (this->registered)
		std::cout << "Client is already registered." << std::endl;


}
void Client::check_if_registered() {
	// try to open me.info file, if this exists, then this->registed = true, and load keys


}
void Client::start() {
// send registeration request -> receive msg, send key, receive key, send file encrypted, receive OK
	if (!this->registered) {
		this->requests_handler.send_register_request();
		this->requests_handler.receive_register_response();
		this->requests_handler.send_key();
		this->requests_handler.receive_key();
	}
	else {
		this->requests_handler.send_login_request();
		this->requests_handler.receive_login_response();

	}
	this->requests_handler.send_encrypted_file();

}




	int main2() {
		// test_messages();
		//test_sending();





		return 0;
	}
