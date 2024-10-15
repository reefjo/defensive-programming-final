#ifndef ENDIANNESS_H
#define ENDIANNESS_H

#include <cstdint>

namespace Endianness {
    bool is_little_endian();  
    uint16_t to_little_endian(uint16_t value);
    uint32_t to_little_endian(uint32_t value);
}

#endif
