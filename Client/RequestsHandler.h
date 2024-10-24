#ifndef REQUESTS_HANDLER_H
#define REQUESTS_HANDLER_H

#include <boost/asio.hpp>
#include <iostream>
#include "RequestPacket.h"
#include "Base64Wrapper.h"
#include "RSAWrapper.h"
#include "AESWrapper.h"
#include "Protocol.h"
#include <fstream>   // For file handling
#include <algorithm>    // For utility functions like std::min
#include <optional>
#include <iomanip>  // For formatting outputs (debugging)
#include "FileHandler.h"
#include "Endianness.h"
#include "cksum.h"


// Class to handle all client requests (e.g., registration, login, file transfer).
// Manages communication with the server, request sending, and response processing.
class RequestsHandler {
private:
    boost::asio::io_context io_context;  
    boost::asio::ip::tcp::resolver resolver;  
    boost::asio::ip::tcp::socket socket;  // Socket to communicate with the server
    std::string client_id;  
    std::string client_name;  
    uint8_t client_version;  

    // Helper functions

    // Receive and unpack the response header from the server
    ResponseHeader unpack_response_header();

    // Get the client ID from the server's registration response
    std::optional<std::string> get_register_response_id();

    // Send an authentication request (login or registration)
    void send_authenticate_request(uint16_t code);

    // Extract a 32-bit unsigned integer from a byte vector at indices i to i + 3
    uint32_t get_uint32_from_vec(const std::vector<uint8_t>& vec, uint32_t i);

    // Extract a 16-bit unsigned integer from a byte vector at indices i to i + 1
    uint16_t get_uint16_from_vec(const std::vector<uint8_t>& vec, uint32_t i);

    // Send a file to the server, encrypted with an AES key
    void send_file(std::string file_name, const std::string aes_key);

    // Get the file checksum sent back by the server
    uint32_t get_send_file_response_crc();

    // Send an acknowledgment after receiving the checksum
    void send_ack_after_crc(const std::string file_name, uint16_t code);

    // Receive acknowledgment from the server
    void get_ack_from_server();

    // Handle the login response from the server, return encrypted AES key
    std::optional<std::string> get_login_response();

public:
    // Constructor that initializes the handler with client data and establishes a connection with the server
    RequestsHandler(std::string client_id, std::string client_name, uint8_t client_version);

    // Send the public key (RSA) to the server
    void send_public_key(const std::string public_key);

    // Receive the encrypted AES key from the server
    std::string get_encrypted_aes(const std::string private_rsa_key);

    // Handle the file transfer process (sending files with encryption)
    void handle_send_file(std::string file_name, const std::string aes_key);

    // Perform login and retrieve the AES key from the server
    std::optional<std::string> login_and_get_aes(std::string private_rsa_key);

    // Register the client and receive the client ID from the server
    std::string register_and_get_id();

    // Set the client ID manually (useful after registration)
    void set_id(std::string client_id);

    // Close the connection with the server
    void close_connection();
};

#endif // REQUESTS_HANDLER_H
