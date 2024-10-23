#ifndef CKSUM_H
#define CKSUM_H
#include <iostream>
#include <fstream>
#include <ostream>
#include <cstdio>
#include <vector>
#include <iterator>
#include <filesystem>
#include <string>

unsigned long get_file_checksum(std::string fname);
unsigned long memcrc(char* b, size_t n);



#endif