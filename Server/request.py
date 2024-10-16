from protocol_constants import CLIENT_ID_SIZE, CODE_SIZE, CLIENT_VERSION_SIZE, PAYLOAD_SIZE
import socket


class Request:
    def __init__(self):
        self.client_id = b""
        self.client_version = 0
        self.code = 0
        self.payload_size = 0

    def parse_from_socket(self, conn) -> None:
        try:
            self.client_id = conn.recv(CLIENT_ID_SIZE)
            self.client_version = int.from_bytes(conn.recv(CLIENT_VERSION_SIZE), 'little')
            self.code = int.from_bytes(conn.recv(CODE_SIZE), 'little')
            self.payload_size = int.from_bytes(conn.recv(PAYLOAD_SIZE), 'little')
            print(
                f"parsed from client: {self.client_id = }, {self.client_version = }, {self.code = }, {self.payload_size = }")

        except socket.error as e:
            print(f"Socket error: {e}")
        except Exception as e:
            print(f"General error during parsing: {e}")
