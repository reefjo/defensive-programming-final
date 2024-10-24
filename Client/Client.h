#ifndef CLIENT_H
#define CLIENT_H
#include <string>
#include <iostream>
#include "RequestsHandler.h" 
#include <optional> //  for std::optional, if i will choose to use it (delete this line if not!)
#include "Protocol.h"
#include "Base64Wrapper.h"
#include <iostream>
#include <fstream> 
#include <string>    // for std::string, std::getline, std::stoi
#include <boost/asio.hpp>
#include "FileHandler.h"
#include "Client.h"

const uint8_t CLIENT_VERSION = 3;

class Client {
private:
	std::string client_name;
	std::string client_id;
	std::string aes_key;
	std::string private_rsa_key;
	std::string file_path;
	RequestsHandler requests_handler;
	bool registered;

	// Private constructor that takes the necessary parameters
	Client(std::tuple<std::string, std::string, std::string, std::string>, std::string);

	void load_stored_credentials();
	std::string generate_keys ();
	bool attempt_login();
	bool attempt_register();
	std::string hex_to_binary(const std::string& hex_str);
	
public:
	Client(); // Constructor
	void start(); // Method to start the client

};

#endif