#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <stdint.h>

// Requests

constexpr uint8_t ID_SIZE = 16;
constexpr uint8_t CLIENT_NAME_SIZE = 255;
constexpr uint16_t REGISTER_REQUEST_CODE = 825;
constexpr uint16_t SEND_KEY_REQUEST_CODE = 826;
constexpr uint8_t REGISTER_REQUEST_SIZE = 255;
constexpr const char DEFAULT_ID[] = "aaaaaaaaaaaaaaaa";
constexpr uint8_t NUM_OF_TRIALS = 3;
constexpr const char PRIVATE_KEY_FILE_NAME[] = "priv.key";



// Responses
constexpr uint16_t REGISTER_SUCCESS_CODE = 1600;
constexpr uint16_t REGISTER_FAIL_CODE = 1601;
// Response header sizes?
constexpr uint8_t SERVER_VERSION_SIZE = 1;
constexpr uint8_t SERVER_RESPONSE_SIZE = 2;
constexpr uint8_t SERVER_RESPONSE_PAYLOAD_SIZE = 4;
constexpr uint8_t SERVER_RESPONSE_HEADER_SIZE = SERVER_VERSION_SIZE + SERVER_RESPONSE_SIZE + SERVER_RESPONSE_PAYLOAD_SIZE;



#endif

