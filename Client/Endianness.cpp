#include "Endianness.h"

// Helper functions to convert little<->big (works both ways)
uint16_t convert(uint16_t value) {
    // Converts value between big-endain and little endian (both ways)
    return (value >> 8) | (value << 8);
}

// Helper functions to convert little<->big (works both ways)
uint32_t convert(uint32_t value) {
    return ((value >> 24) & 0xFF) |
        ((value >> 8) & 0xFF00) |
        ((value << 8) & 0xFF0000) |
        ((value << 24) & 0xFF000000); 
}

// Detect if the machine is little or big endian
bool is_little_endian() {
    uint16_t num = 0x1;
    return *(reinterpret_cast<uint8_t*>(&num)) == 0x1;
}

namespace Endianness {



    // Convert to little-endian format
    uint16_t to_little_endian(uint16_t value) {
        if (is_little_endian()) {
            return value; // No conversion needed
        }
        return convert(value);
    }

    uint32_t to_little_endian(uint32_t value) {
        if (is_little_endian()) {
            return value; // No conversion needed
        }
        return convert(value);
    }
    
    // Convert to little-endian format
    uint16_t  from_little_to_native(uint16_t value) {
        if (is_little_endian()) {
            return value; // No conversion needed
        }
        return convert(value);
    }

    uint32_t from_little_to_native(uint32_t value) {
        if (is_little_endian()) {
            return value; // No conversion needed
        }
        return convert(value);
    }
}
