#ifndef PROTOCOL_H
#define PROTOCOL_H


#include <stdint.h>
// Requests


constexpr uint8_t ID_SIZE = 16;
constexpr uint8_t CLIENT_NAME_SIZE = 255;
constexpr uint8_t FILE_NAME_SIZE = 255;
constexpr uint16_t REGISTER_REQUEST_CODE = 825;
constexpr uint16_t LOGIN_REQUEST_CODE = 827;

constexpr uint16_t SEND_KEY_REQUEST_CODE = 826;
constexpr uint16_t SEND_FILE_REQUEST_CODE = 828;
constexpr uint16_t VALID_CRC_REQUEST_CODE = 900;
constexpr uint16_t INVALID_CRC_REQUEST_CODE = 901;
constexpr uint16_t INVALID_CRC_FINAL_REQUEST_CODE = 902;  // invalid final : the client won't try sending the file anymore

constexpr uint8_t REGISTER_REQUEST_SIZE = 255;
constexpr const char DEFAULT_ID[] = "aaaaaaaaaaaaaaaa";
constexpr uint8_t NUM_OF_TRIALS = 3;
constexpr const char PRIVATE_KEY_FILE_NAME[] = "priv.key";
constexpr const char ME_INFO_FILE_NAME[] = "me.info";
constexpr const char TRANSFER_FILE_NAME[] = "transfer.info";
constexpr const char TEST_FILE_NAME[] = "send_this.txt";
constexpr const uint16_t BUFFER_SIZE = 1024;



// Responses
constexpr uint16_t REGISTER_SUCCESS_CODE = 1600;
constexpr uint16_t REGISTER_FAIL_CODE = 1601;
constexpr uint16_t RECEIVED_KEY_SUCCESS_CODE = 1602;
constexpr uint16_t RECEIVED_FILE_SUCCESS_CODE = 1603;
constexpr uint16_t LOGIN_SUCCESS_CODE = 1605;
constexpr uint16_t SERVER_OK_CODE = 1604;
// Response header sizes?
constexpr uint8_t SERVER_VERSION_SIZE = 1;
constexpr uint8_t SERVER_RESPONSE_SIZE = 2;
constexpr uint8_t SERVER_RESPONSE_PAYLOAD_SIZE = 4;
constexpr uint8_t SERVER_RESPONSE_HEADER_SIZE = SERVER_VERSION_SIZE + SERVER_RESPONSE_SIZE + SERVER_RESPONSE_PAYLOAD_SIZE;

constexpr uint8_t CHECKSUM_SIZE = 4;

#endif

