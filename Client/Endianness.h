#ifndef ENDIANNESS_H
#define ENDIANNESS_H

#include <cstdint>

namespace Endianness {

    /**
     * @brief Converts a 16-bit value to little-endian format.
     *
     * If the machine is already little-endian, the value is returned unchanged.
     * Otherwise, the function converts it from big-endian to little-endian.
     *
     * @param value The 16-bit unsigned integer to convert.
     * @return uint16_t The value in little-endian format.
     */
    uint16_t to_little_endian(uint16_t value);

    /**
     * @brief Converts a 32-bit value to little-endian format.
     *
     * If the machine is already little-endian, the value is returned unchanged.
     * Otherwise, the function converts it from big-endian to little-endian.
     *
     * @param value The 32-bit unsigned integer to convert.
     * @return uint32_t The value in little-endian format.
     */
    uint32_t to_little_endian(uint32_t value);

    /**
     * @brief Converts a 16-bit value from little-endian to native machine format.
     *
     * If the machine is little-endian, the value is returned unchanged.
     * Otherwise, the function converts it from little-endian to big-endian.
     *
     * @param value The 16-bit unsigned integer in little-endian format.
     * @return uint16_t The value converted to the machine's native format.
     */
    uint16_t from_little_to_native(uint16_t value);

    /**
     * @brief Converts a 32-bit value from little-endian to native machine format.
     *
     * If the machine is little-endian, the value is returned unchanged.
     * Otherwise, the function converts it from little-endian to big-endian.
     *
     * @param value The 32-bit unsigned integer in little-endian format.
     * @return uint32_t The value converted to the machine's native format.
     */
    uint32_t from_little_to_native(uint32_t value);
}

#endif
