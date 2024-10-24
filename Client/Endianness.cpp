#include "Endianness.h"

// Helper function to convert a 16-bit value between big-endian and little-endian
uint16_t convert(uint16_t value) {
    // Swaps the byte order by shifting the bits
    return (value >> 8) | (value << 8);
}

// Helper function to convert a 32-bit value between big-endian and little-endian
uint32_t convert(uint32_t value) {
    // Swaps the byte order by shifting the bits
    return ((value >> 24) & 0xFF) |
        ((value >> 8) & 0xFF00) |
        ((value << 8) & 0xFF0000) |
        ((value << 24) & 0xFF000000);
}

// Helper function to detect if the current machine is little-endian
bool is_little_endian() {
    uint16_t num = 0x1;
    // If the first byte of the 16-bit number is 0x1, the machine is little-endian
    return *(reinterpret_cast<uint8_t*>(&num)) == 0x1;
}

namespace Endianness {

    // Converts a 16-bit value to little-endian format
    uint16_t to_little_endian(uint16_t value) {
        if (is_little_endian()) {
            // If the system is little-endian, no conversion is needed
            return value;
        }
        // Otherwise, convert the value to little-endian format
        return convert(value);
    }

    // Converts a 32-bit value to little-endian format
    uint32_t to_little_endian(uint32_t value) {
        if (is_little_endian()) {
            // If the system is little-endian, no conversion is needed
            return value;
        }
        // Otherwise, convert the value to little-endian format
        return convert(value);
    }

    // Converts a 16-bit value from little-endian format to native machine format
    uint16_t from_little_to_native(uint16_t value) {
        if (is_little_endian()) {
            // If the system is little-endian, no conversion is needed
            return value;
        }
        // Otherwise, convert the value from little-endian to native format
        return convert(value);
    }

    // Converts a 32-bit value from little-endian format to native machine format
    uint32_t from_little_to_native(uint32_t value) {
        if (is_little_endian()) {
            // If the system is little-endian, no conversion is needed
            return value;
        }
        // Otherwise, convert the value from little-endian to native format
        return convert(value);
    }
}
