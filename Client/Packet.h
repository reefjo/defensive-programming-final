#ifndef PACKET_H
#define PACKET_H
#include "Header.h"
#include "Payload.h"
class Packet {
private:
	Header h;
	Payload p;
};



#endif
