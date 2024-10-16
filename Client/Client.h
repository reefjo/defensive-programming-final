#ifndef CLIENT_H
#define CLIENT_H
#include <string>
#include <iostream>
#include "RequestsHandler.h" // Assuming you have a RequestsHandler class
#include <optional> // Include for std::optional

const uint8_t CLIENT_VERSION = 3;

class Client {
private:
	std::string client_name;
	std::string client_id;
	std::string aes_key;
	std::string file_path;
	RequestsHandler requests_handler;
	bool registered;

	// Private constructor that takes the necessary parameters
	Client(std::tuple<std::string, std::string, std::string, std::string>);

	//void read_transfer_and_connect();
	void authenticate();
public:
	Client(); // Constructor
	void start(); // Method to start the client

};

#endif