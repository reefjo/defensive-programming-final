#ifndef REQUESTS_HANDLER_H
#define REQUESTS_HANDLER_H
#include <boost/asio.hpp>
#include <iostream>
#include "RequestPacket.h"
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
	ResponseHeader unpack_response_header();
	void send_register_request();
	bool handle_register_response();



public:
	RequestsHandler(std::string, std::string, uint8_t);
	void handle_registration();
};


#endif