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
