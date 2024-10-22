#include "Payload.h"

RegisterPayload::RegisterPayload(const std::string client_name)
	: client_name(client_name){}

std::vector<uint8_t> RegisterPayload::serialize () const{
	std::vector<uint8_t> res;
	for (char c : this->client_name) {
		res.push_back(static_cast<uint8_t>(c));
	}
	return res;

}

SendKeyPayload::SendKeyPayload(const std::string client_name, const std::string public_key)
	:client_name(client_name), public_key(public_key){}

std::vector<uint8_t> SendKeyPayload::serialize() const {
	std::vector<uint8_t> res;
	for (char c : this->client_name) {
		res.push_back(static_cast<uint8_t>(c));
	}
	for (char c : this->public_key) {
		res.push_back(static_cast<uint8_t>(c));
	}
	return res;
}


SendFilePayload::SendFilePayload(uint32_t file_size, uint32_t orig_file_size, uint16_t packet_number,
	uint16_t total_packets, std::string file_name, std::string data)
	:file_size(file_size), orig_file_size(orig_file_size), packet_number(packet_number),
	total_packets(total_packets), file_name(file_name), data(data){}

std::vector<uint8_t> SendFilePayload::serialize() const {
	std::vector<uint8_t> res;
	uint32_t file_size_le = Endianness::to_little_endian(this->file_size);
	uint32_t orig_file_size_le = Endianness::to_little_endian(this->orig_file_size);
	uint16_t packet_number_le = Endianness::to_little_endian(this->packet_number);
	uint16_t total_packets_le = Endianness::to_little_endian(this->total_packets);

	// Serialize file_size (4 bytes)
	for (int i = 0; i < sizeof(uint32_t); i++)
		res.push_back((file_size_le >> 8 * i) & 0xFF);

	// Serialize orig_file_size (4 bytes)
	for (int i = 0; i < sizeof(uint32_t); i++)
		res.push_back((orig_file_size_le >> 8 * i) & 0xFF);

	// Serialize packet_number (2 bytes)
	for (int i = 0; i < sizeof(uint16_t); i++)
		res.push_back((packet_number_le >> 8 * i) & 0xFF);

	// Serialize total_packets (2 bytes)
	for (int i = 0; i < sizeof(uint16_t); i++)
		res.push_back((total_packets_le >> 8 * i) & 0xFF);

	// Serialize file name
	for (char c : file_name)
		res.push_back(static_cast<uint8_t>(c));

	// Serialize data
	for (char c : data)
		res.push_back(static_cast<uint8_t>(c));

	return res;
};