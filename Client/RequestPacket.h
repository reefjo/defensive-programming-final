#ifndef PACKET_H
#define PACKET_H


#include "Header.h"
#include "Payload.h"
#include <memory>
#include <vector>       // For std::vector


// Class representing a complete packet, containing a header and a payload

class Packet {
private:
    RequestHeader h;
    std::unique_ptr<Payload> p;  // Smart pointer to Payload (RegistrationPayload, FilePayload, etc.)

public:
    Packet(RequestHeader, std::unique_ptr<Payload>);

    // Serializes the packet by combining the serialized header and payload into a byte array
    std::vector<uint8_t> serialize() const;
};



#endif
