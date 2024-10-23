#ifndef REQUESTS_HANDLER_H
#define REQUESTS_HANDLER_H
#include <boost/asio.hpp>
#include <iostream>
#include "RequestPacket.h"
#include "Base64Wrapper.h"
#include "RSAWrapper.h"
#include "AESWrapper.h"
#include "Protocol.h"
#include <fstream>   // For std::ifstream, std::ofstream
#include <algorithm>    // std::min


#include <iostream>
#include <iomanip>
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
	uint32_t get_uint32_from_vec(const std::vector<uint8_t>& vec, uint32_t i);
	uint16_t get_uint16_from_vec(const std::vector<uint8_t>& vec, uint32_t i);
	void send_file(std::string file_name, const std::string aes_key);
	uint32_t get_send_file_response_crc();
	void send_ack_after_crc(const std:: string, uint16_t);
	void get_ack_from_server();


public:
	RequestsHandler(std::string, std::string, uint8_t);
	void handle_registration();
	void send_public_key(const std::string);
	std::string get_encrypted_aes(const std::string);
	void handle_send_file(std::string, const std::string);


};


#endif