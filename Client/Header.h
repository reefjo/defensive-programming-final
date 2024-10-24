#ifndef HEADER_H
#define HEADER_H
#include <stdint.h>
#include <vector>
#include <string>
#include "Endianness.h"


// Base class for headers (both request and response headers)
class Header {
public:
	virtual ~Header() = default;
};

// Represents the header of a request from the client to the server
class RequestHeader : public Header {
private:
	const std::string client_id;
	uint8_t client_version;
	uint16_t code;   // Code representing the type of request
	uint32_t payload_size;

public:
	// Constructor to initialize the request header fields
	RequestHeader(const std::string id, uint8_t version, uint16_t code, uint32_t payload_size);

	// Getters for accessing private fields
	const std::string  get_client_id() const;
	uint8_t get_client_version() const;
	uint16_t get_request_code() const;
	uint32_t get_payload_size() const;

	// Serializes the RequestHeader into a byte array for transmission
	std::vector<uint8_t> serialize() const; 
};

// Represents the header of a response from the server to the client
class ResponseHeader : public Header {
private:
	uint8_t server_version;
	uint16_t response_code;  // Code representing the type of response
	uint32_t payload_size;

public:
	// Constructor to initialize the response header fields
	ResponseHeader(uint8_t version, uint16_t code, uint32_t size);

	// Getters for accessing private fields
	uint8_t get_server_version() const;
	uint16_t get_response_code() const;
	uint32_t get_payload_size() const;

};

#endif