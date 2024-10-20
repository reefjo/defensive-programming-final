#ifndef PAYLOAD_H
#define PAYLOAD_H
#include <vector>
#include <string>
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


#endif