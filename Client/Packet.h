#ifndef PACKET_H
#define PACKET_H
#include "Header.h"
#include "Payload.h"
class Packet {
private:
	Header* h;  // Polymorphic header(can be either RequestHeader or ResponseHeader)
	Payload p;
};



#endif
