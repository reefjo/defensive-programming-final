from protocol_constants import SERVER_VERSION, CLIENT_VERSION_SIZE, CODE_SIZE, PAYLOAD_SIZE

import socket


class Response:
    def __init__(self):
        self.version = SERVER_VERSION
        self.code = 0
        self.payload = b""

    def send_response(self, conn) -> None:
        print(f"Trying to send response:")
        try:
            response_message = self._construct_response_message()
            conn.sendall(response_message)
            print(f"Response sent to client\npayload: {self.payload}")
        except socket.error as e:
            print(f"Error sending response: {e}")
        except Exception as e:
            print(f"General error during sending response: {e}")

    def _construct_response_message(self):
        payload_size = len(self.payload)
        response_header = self.version.to_bytes(CLIENT_VERSION_SIZE, 'little')
        response_header += self.code.to_bytes(CODE_SIZE, 'little')
        response_header += payload_size.to_bytes(PAYLOAD_SIZE, 'little')
        return response_header + self.payload
