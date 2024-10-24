from protocol_constants import CODE_SIZE, PAYLOAD_SIZE, SERVER_VERSION_SIZE, SERVER_VERSION

import socket


class Response:
    def __init__(self):
        self.version = SERVER_VERSION
        self.code = 0
        self.payload = b""

    def send_response(self, conn) -> None:
        response_message = self._construct_response_message()
        conn.sendall(response_message)

    def _construct_response_message(self):
        payload_size = len(self.payload)
        response_header = self.version.to_bytes(SERVER_VERSION_SIZE, 'little')
        response_header += self.code.to_bytes(CODE_SIZE, 'little')
        response_header += payload_size.to_bytes(PAYLOAD_SIZE, 'little')
        return response_header + self.payload
