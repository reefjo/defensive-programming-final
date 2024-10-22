#ifndef PAYLOAD_H
#define PAYLOAD_H
#include <vector>
#include <string>
#include "Endianness.h"
class Payload {
public:
	virtual ~Payload() = default;
	// Each payload subclass must implement this method to serialize its contents
	virtual std::vector<uint8_t> serialize() const = 0;
};



class RegisterPayload : public Payload {
private:
	std::string client_name;  // should be size 255
public:
	//constructor
	RegisterPayload(const std::string);
	std::vector<uint8_t> serialize() const override;
	// no need getters & setters i think

};

class SendKeyPayload : public Payload {
private:
	std::string client_name;
	std::string public_key;
public:
	SendKeyPayload(const std::string, const std::string);
	std::vector<uint8_t> serialize() const override;
	

};
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
	std::vector<uint8_t> serialize() const override;

};

#endif