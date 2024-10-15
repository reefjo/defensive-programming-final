#include "Endianness.h"

namespace Endianness {

    // Detect if the machine is little or big endian
    bool is_little_endian() {
        uint16_t num = 0x1;
        return *(reinterpret_cast<uint8_t*>(&num)) == 0x1;
    }

    // Convert to little-endian format
    uint16_t to_little_endian(uint16_t value) {
        if (is_little_endian()) {
            return value; // No conversion needed
        }
        else {
            return (value >> 8) | (value << 8); // Swap bytes for big-endian machines
        }
    }

    uint32_t to_little_endian(uint32_t value) {
        if (is_little_endian()) {
            return value; // No conversion needed
        }
        else {
            return ((value >> 24) & 0xFF) |
                ((value >> 8) & 0xFF00) |
                ((value << 8) & 0xFF0000) |
                ((value << 24) & 0xFF000000); // Swap bytes for big-endian machines
        }
    }
}
