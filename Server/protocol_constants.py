BUFFER_SIZE = 1024
CLIENT_ID_SIZE = 16
CLIENT_VERSION_SIZE = 1
SERVER_VERSION_SIZE = 1
CODE_SIZE = 2
PAYLOAD_SIZE = 4
CLIENT_NAME_SIZE = 255
REGISTER_CODE = 825
SEND_FILE_CODE = 828
SEND_KEY_CODE = 826
REQUEST_HEADER_SIZE = CLIENT_ID_SIZE + CLIENT_VERSION_SIZE + CODE_SIZE + PAYLOAD_SIZE
SERVER_VERSION = 3
REGISTER_SUCCESS_CODE = 1600
REGISTER_FAILED_CODE = 1601
RECEIVED_KEY_SUCCESS_CODE = 1602
RECEIVED_FILE_SUCCESS_CODE = 1603
GENERAL_FAIL_CODE = 1607
REQUEST_HEADER_STRUCTURE = '<16sBHI' # Format: 16-byte string, 1-byte uint8, 2-byte uint16, 4-byte uint32
SEND_KEY_REQUEST_STRUCTURE = '<255s160s'
SEND_FILE_REQUEST_STRUCTURE = '<IIHH255s'
KEY_SIZE = 160
SEND_KEY_PAYLOAD_SIZE = CLIENT_NAME_SIZE + KEY_SIZE
AES_KEY_SIZE = 32
BUFFER_SIZE = 1024

# for file sending
CONTENT_SIZE = 4
ORIG_FILE_SIZE = 4
PACKET_NUMBER_SIZE = 2
TOTAL_PACKETS_SIZE = 2
FILE_NAME_SIZE = 255
FILE_PAYLOAD_SIZE_WITHOUT_DATA = CONTENT_SIZE + ORIG_FILE_SIZE + PACKET_NUMBER_SIZE + TOTAL_PACKETS_SIZE + FILE_NAME_SIZE



