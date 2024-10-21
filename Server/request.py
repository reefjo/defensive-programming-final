import struct

from protocol_constants import CLIENT_ID_SIZE, CODE_SIZE, CLIENT_VERSION_SIZE, PAYLOAD_SIZE, REQUEST_HEADER_SIZE, REQUEST_HEADER_STRUCTURE
import socket


class RequestHeader:
    def __init__(self):
        self.client_id = b""
        self.client_version = 0
        self.code = 0
        self.payload_size = 0

    def parse_from_socket(self, conn) -> None:
        # Parse ALL the request
        '''
        data = conn.recv(REQUEST_HEADER_SIZE)
        self.client_id = data[:CLIENT_ID_SIZE]
        if not self.client_id:
            raise ConnectionError("Client disconnected while receiving client version.")
        elif len(data) < REQUEST_HEADER_SIZE:
            raise ConnectionError("Insufficent data received from client (less than expected)")
        self.client_version, self.code, self.payload_size = struct.unpack(REQUEST_HEADER_STRUCTURE, data[CLIENT_ID_SIZE:])
'''

        self.client_id = conn.recv(CLIENT_ID_SIZE)
        if not self.client_id:
            raise ConnectionError("Client disconnected while receiving client version.")
        print(f"client_id size : {len(self.client_id) = }")
        self.client_version = int.from_bytes(conn.recv(CLIENT_VERSION_SIZE), 'little')
        if not self.client_version:
            raise ConnectionError("Client disconnected while receiving client version.")
        self.code = int.from_bytes(conn.recv(CODE_SIZE), 'little')
        if not self.code:
            raise ConnectionError("Client disconnected while receiving client version.")
        self.payload_size = int.from_bytes(conn.recv(PAYLOAD_SIZE), 'little')
        if not self.payload_size:
            raise ConnectionError("Client disconnected while receiving client version.")
        print(
            f"parsed from client: {self.client_id = }, {self.client_version = }, {self.code = }, {self.payload_size = }")

