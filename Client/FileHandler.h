#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>
#include <tuple>
#include <fstream>   // For file handling
#include <boost/asio.hpp>
#include <optional>
#include "Protocol.h"

// Function to write client information (client name and ID) to the "me.info" file
void write_me_info(const std::string client_name, const std::string client_id);

// Function to store the AES key in the appropriate file after key exchange
void put_key_in_files(std::string aes_key);

// Function to read transfer information from the "transfer.info" file.
// Returns a tuple containing server IP, server port, client name, and file path.
std::tuple<std::string, std::string, std::string, std::string> read_transfer_file();

#endif // FILE_HANDLER_H
