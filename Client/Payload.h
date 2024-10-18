#ifndef PAYLOAD_H
#define PAYLOAD_H

class Payload {
public:
	virtual ~Payload() = default;
	// Each payload must implement this method to serialize its contents
	virtual std::vector<uint8_t> serialize() const = 0;
};



class RegisterPayload : public Payload {
private:
	std::string client_name;
public:
	RegisterPayload
	std::vector<uint8_t> serialize();

}




class RequestHeader : public Header {
private:
	const std::string client_id;
	uint8_t client_version;
	uint16_t code;
	uint32_t payload_size;

public:
	// Constructor
	RequestHeader(const std::string id, uint8_t version, uint16_t code, uint32_t payload_size);

	// Getters (might not use them)
	const std::string  get_client_id() const;  // const to indicate that this function doesnt modify this object
	uint8_t get_client_version() const;
	uint16_t get_request_code() const;
	uint32_t get_payload_size() const;

	std::vector<uint8_t> serialize() const override;  // Serialize to byte array


#endif