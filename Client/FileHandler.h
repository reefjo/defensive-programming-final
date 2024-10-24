#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>
#include <tuple>
#include <fstream>   // For std::ifstream, std::ofstream
#include <boost/asio.hpp>
#include <optional>
#include "Protocol.h"



void write_me_info(const std::string, const std::string);
void put_key_in_files(std::string);

std::tuple<std::string, std::string, std::string, std::string> read_transfer_file();




#endif 
