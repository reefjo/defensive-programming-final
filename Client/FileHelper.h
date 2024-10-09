
#pragma once
#include <iostream>
#include <fstream> 
#include <string>
#include <tuple>  // for std::tuple
// commetn to see if .gitignore adds this file 

const std::string TRANSFER_FILE_NAME = "transfer.info";

std::tuple<std::string, std::string, std::string, std::string> readTransferFile();
