#include "Packet.h"

Packet::Packet(std::unique_ptr<Header> header, std::unique_ptr<Payload> payload)
	:h(std::move(header)), p(std::move(payload)){}

std::vector<uint8_t> Packet::serialize() const{
	std::vector<uint8_t> res;
	std::vector<uint8_t> header_serialized = this->h->serialize();
	std::vector<uint8_t> payload_serialized = this->p->serialize();

	res.insert(res.end(), header_serialized.begin(), header_serialized.end());
	res.insert(res.end(), payload_serialized.begin(), payload_serialized.end());



	return res;
}