#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>
#include <tuple>
#include <boost/asio.hpp>
#include <optional>

#define TRANSFER_FILE_NAME "transfer.info"




// Function to read server IP, port, client name, and file path from info.transfer file
//std::tuple<std::string, std::string, std::string, std::string> read_transfer_file();

// Function to send file to server using Boost Asio
void sendFile(const std::string& fileName, boost::asio::ip::tcp::socket& socket);

// Function to receive file from server using Boost Asio
void receiveFile(const std::string& fileName, boost::asio::ip::tcp::socket& socket);

void write_me_info(const std::string, const std::string);

std::vector<std::string > read_key_from_file();
std::tuple<std::string, std::string, std::string, std::string> read_transfer_file();




#endif 
