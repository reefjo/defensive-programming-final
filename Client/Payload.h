#ifndef PAYLOAD_H
#define PAYLOAD_H


#include <vector>
#include <string>
#include "Endianness.h"


// Abstract base class representing a payload in a network message
class Payload {
public:
	virtual ~Payload() = default;

	// Serializes the payload content into a byte array
	virtual std::vector<uint8_t> serialize() const = 0;
};



// Represents the payload for a client registration request
class RegisterPayload : public Payload {
private:
	std::string client_name;  
public:
	RegisterPayload(const std::string);

	// Serializes the client_name into a byte array
	std::vector<uint8_t> serialize() const override;
};

// Represents the payload for sending a public key to the server
class SendKeyPayload : public Payload {
private:
	std::string client_name;
	std::string public_key;
public:
	SendKeyPayload(const std::string, const std::string);

	// Serializes the client_name and public_key into a byte array
	std::vector<uint8_t> serialize() const override;
};


// Represents the payload for sending an encrypted file to the server
class SendFilePayload : public Payload {
private:
	uint32_t file_size;  // file size  AFTER encryption
	uint32_t orig_file_size;
	uint16_t packet_number;
	uint16_t total_packets;  // total packets sent so far
	std::string file_name;
	std::string data;  // the acutal data sent
public:
	SendFilePayload(uint32_t, uint32_t, uint16_t, uint16_t, std::string, std::string);

	// Serializes the file-related data into a byte array
	std::vector<uint8_t> serialize() const override;

};

#endif