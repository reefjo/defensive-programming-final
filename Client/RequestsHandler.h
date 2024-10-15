#ifndef REQUESTS_HANDLER_H
#define REQUESTS_HANDLER_H
#include <boost/asio.hpp>
#include <iostream>
#define CLIENT_NAME_SIZE 255
#define CLIENT_ID_SIZE 16
#define TRANSFER_FILE_NAME "transfer.info"
#define REIGSTER_REQUEST_CODE 825
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


public:
	RequestsHandler(std::string, uint8_t);
	void send_register_request();
	void load_id_version(std::vector < uint8_t> &);
};


#endif