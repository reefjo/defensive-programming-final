#ifndef PACKET_H
#define PACKET_H

#include "Header.h"
#include "Payload.h"
#include <memory>
#include <vector>       // For std::vector

class Packet {
private:
    RequestHeader h;  
    std::unique_ptr<Payload> p;  // Smart pointer to Payload (RegistrationPayload, FilePayload, etc.)
public:
    Packet(RequestHeader, std::unique_ptr<Payload>);
    std::vector<uint8_t> serialize() const;
};



#endif
