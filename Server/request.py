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
        # Read the entire header in one go
        data = conn.recv(REQUEST_HEADER_SIZE)
        if not data:
            raise ConnectionError("Client disconnected socket")
        elif len(data) < REQUEST_HEADER_SIZE:
            raise ConnectionError("Insufficient data received from client (less than expected)")

        # Extract fields from the received data
        self.client_id = data[:CLIENT_ID_SIZE]
        if not self.client_id:
            raise ConnectionError("Client disconnected while receiving client ID.")

        # Unpack remaining header data using struct (assuming appropriate format in REQUEST_HEADER_STRUCTURE)
        self.client_version, self.code, self.payload_size = struct.unpack(
            '<BHI', data[CLIENT_ID_SIZE:]
        )
        print(
            f"Parsed from client: client_id={self.client_id}, client_version={self.client_version}, code={self.code}, payload_size={self.payload_size}")

