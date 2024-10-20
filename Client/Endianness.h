#ifndef ENDIANNESS_H
#define ENDIANNESS_H

#include <cstdint>

namespace Endianness {
    uint16_t to_little_endian(uint16_t value);
    uint32_t to_little_endian(uint32_t value);
    uint16_t from_little_to_native(uint16_t value);
    uint32_t from_little_to_native(uint32_t value);
}

#endif
