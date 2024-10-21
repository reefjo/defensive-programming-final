#include "Header.h"

RequestHeader::RequestHeader(const std::string id, uint8_t version, uint16_t code, uint32_t size)
	: client_id(id), client_version(version), code(code), payload_size(size) {}

// Getters for RequestHeader
const std::string RequestHeader::get_client_id() const { return client_id; }
uint8_t RequestHeader::get_client_version() const { return client_version; }
uint16_t RequestHeader::get_request_code() const { return code; }
uint32_t RequestHeader::get_payload_size() const { return payload_size; }

// Serialize RequestHeader into a byte vector
std::vector<uint8_t> RequestHeader::serialize() const {
	std::vector<uint8_t> res;
	// Serialize client_id (assuming 16-byte length)
	for (char c : client_id) {
		res.push_back(static_cast<uint8_t>(c));
	}

	// put client version
	res.push_back(this->client_version);
	// put code
	uint16_t code_le = Endianness::to_little_endian(code);
	res.push_back(static_cast<uint8_t>(code_le & 0xFF));      // Lower byte
	res.push_back(static_cast<uint8_t>((code_le >> 8) & 0xFF)); // Upper byte
	//put payload size
	uint32_t size_le = Endianness::to_little_endian(this->payload_size);
	res.push_back(static_cast<uint8_t>(size_le & 0xFF));          // Byte 0
	res.push_back(static_cast<uint8_t>((size_le >> 8) & 0xFF));   // Byte 1
	res.push_back(static_cast<uint8_t>((size_le >> 16) & 0xFF));  // Byte 2
	res.push_back(static_cast<uint8_t>((size_le >> 24) & 0xFF));  // Byte 3

	return res;

}

uint8_t ResponseHeader::get_server_version() const { return this->server_version; }
uint16_t ResponseHeader::get_response_code() const { return this->response_code; }
uint32_t ResponseHeader::get_payload_size() const { return this->payload_size; }

ResponseHeader::ResponseHeader(uint8_t version, uint16_t code, uint32_t size)
	:server_version(version), response_code(code), payload_size(size){}