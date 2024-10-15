#include "RequestsHandler.h"
#include "FileHandler.h"
RequestsHandler::RequestsHandler(std::string client_name, uint8_t client_version)
	: resolver(io_context), socket(io_context), client_name(client_name), client_version(client_version) {

	auto [server_ip, server_port, client_name, file_path] = read_transfer_file();
	if (!is_valid_ip(server_ip)) {
		throw std::runtime_error("\nnot a valid IP address from transfer.info file\n");
	}
	if (!is_valid_port(server_port))
		throw std::runtime_error("\n not a valid port from transfer.info file\n");
	boost::asio::connect(socket, resolver.resolve(server_ip, server_port));

	std::cout << "Connected successfully to IP: " << server_ip << ", Port: " << server_port << std::endl;

	// Initialize client_name to a size of 255
	this->client_name.resize(255, '\0'); // Resize and fill with null characters
	std::cout << "Client name for the request_Handler: " << this->client_name << "Length: " << this->client_name.length() << std::endl;


}

bool RequestsHandler::is_valid_ip(std::string ip) {
	return true; // update this function later

}
bool RequestsHandler::is_valid_port(std::string port) {
	return true;  // update this function later
}

void RequestsHandler::send_register_request() {
	std::vector<uint8_t> arr;
	load_id_version(arr);





}
void RequestsHandler::load_id_version(std::vector < uint8_t> &arr) {
	// load the client id (if exists. and if not - load with junk) up to 16 bytes

	// load the client id
	for (int i = 0; i < CLIENT_ID_SIZE; i++) {
			arr.push_back(this->client_id.at(i));;
	}
	// load the version
	arr.push_back(this->client_version);

	

}