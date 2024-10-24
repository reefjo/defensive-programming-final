#include "RequestPacket.h"

Packet::Packet(RequestHeader header, std::unique_ptr<Payload> payload)
	:h(header), p(std::move(payload)){}


// Serializes the entire Packet into a byte array by first serializing the header and then the payload.
// The resulting byte arrays are concatenated and returned as a single std::vector<uint8_t>.
std::vector<uint8_t> Packet::serialize() const{
	std::vector<uint8_t> res;

	// Serialize the header and payload separately
	std::vector<uint8_t> header_serialized = h.serialize();
	std::vector<uint8_t> payload_serialized = this->p->serialize();

	// Append serialized header to the result
	res.insert(res.end(), header_serialized.begin(), header_serialized.end());

	// Append serialized payload to the result
	res.insert(res.end(), payload_serialized.begin(), payload_serialized.end());

	return res;  // Return the complete serialized packet
}