#ifndef REQUESTS_HANDLER_H
#define REQUESTS_HANDLER_H
#include <boost/asio.hpp>
#include <iostream>
#include "Packet.h"
#define TRANSFER_FILE_NAME "transfer.info"
// This class contains the boost::asio and everything
class RequestsHandler {
private:
	boost::asio::io_context io_context;
	boost::asio::ip::tcp::resolver resolver;
	boost::asio::ip::tcp::socket socket;
	std::string client_id;
	std::string client_name;
	uint8_t client_version;
	bool is_valid_ip(std::string ip);
	bool is_valid_port(std::string port);

	void load_code(std::vector<uint8_t>& arr, uint16_t code);
	void load_payload_size(std::vector<uint8_t>& arr, uint32_t size);



public:
	RequestsHandler(std::string, std::string, uint8_t);
	void send_register_request();
	void receive_register_response();
	void load_id_version(std::vector < uint8_t> &);
};


#endif