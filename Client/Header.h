#ifndef HEADER_H
#define HEADER_H
#include <stdint.h>
#include <vector>
#include <string>
#include "Endianness.h"
class Header {
public:
	virtual std::vector<uint8_t> serialize() const = 0; // why const = 0?
	virtual ~Header() = default;
};

// RequestHeader subclass
class RequestHeader : public Header {
private:
	const std::string client_id;
	uint8_t client_version;
	uint16_t code;
	uint32_t payload_size;
	
public:
	// Constructor
	RequestHeader(const std::string id, uint8_t version, uint16_t code, uint32_t payload_size);

	// Getters (might not use them, delete later?)
	const std::string  get_client_id() const;  // const to indicate that this function doesnt modify this object
	uint8_t get_client_version() const;
	uint16_t get_request_code() const;
	uint32_t get_payload_size() const;
	
	std::vector<uint8_t> serialize() const override;  // Serialize to byte array


};

// ResponseHeader subclass
class ResponseHeader : public Header {
private:
	uint8_t server_version;
	uint16_t code;
	uint32_t payload_size;

public:
	// constructor
	ResponseHeader(uint8_t version, uint16_t code, uint32_t size);
	
	// Getters
	uint8_t get_server_version() const;
	uint16_t get_response_code() const;
	uint32_t get_payload_size() const;

	std::vector<uint8_t> serialize() const override;

};

#endif