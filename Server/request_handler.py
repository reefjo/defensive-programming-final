import socket
import struct
import Crypto
from Crypto.Cipher import PKCS1_OAEP
from Crypto.Random import get_random_bytes
from Crypto.PublicKey import RSA

from request import RequestHeader
from protocol_constants import (
    REGISTER_CODE, SEND_FILE_CODE, CLIENT_NAME_SIZE, BUFFER_SIZE,
    REGISTER_SUCCESS_CODE, REGISTER_FAILED_CODE, SEND_KEY_CODE, SEND_KEY_REQUEST_STRUCTURE, SEND_KEY_PAYLOAD_SIZE,
    AES_KEY_SIZE, KEY_SIZE, GENERAL_FAIL_CODE
)
from database import Database
from response import Response
import uuid


class RequestHandler:
    def __init__(self, conn, db: Database):
        self.conn = conn
        self.db = db
        self.request_header = RequestHeader()
        self.response = Response()

    def handle_request(self):
        self.request_header.parse_from_socket(self.conn)
        print("Request received from client")


        if self.request_header.code == REGISTER_CODE:
            self.handle_register_request()
        elif self.request_header.code == SEND_FILE_CODE:
            self.receive_file("receive_here_server.txt")
        elif self.request_header.code == SEND_KEY_CODE:
            self.handle_send_key_request()
        else:
            print(f"Unknown request code: {self.request_header.code}")
            self.response.code = GENERAL_FAIL_CODE
        # send the response
        print(f"After parsing request, trying to send response")
        self.response.send_response(self.conn)

    def handle_send_key_request(self):
        # Payload : client name (255 bytes), fetch it
        data = self.conn.recv(self.request_header.payload_size)
        client_name, public_key = struct.unpack(SEND_KEY_REQUEST_STRUCTURE, data)

        # import the public key
        rsa_key = RSA.import_key(public_key)

        # Generate AES key
        aes_key = get_random_bytes(AES_KEY_SIZE)

        # Encrypt the AES key with the client's public key
        cipher_rsa = PKCS1_OAEP.new(rsa_key)
        encrypted_aes_key = cipher_rsa.encrypt(aes_key)

        # Dynamically
        client_id = self.request_header.client_id
        self.response.payload = struct.pack('<%ds%ds' % (len(client_id), len(encrypted_aes_key)), client_id, encrypted_aes_key)

    def handle_register_request(self,) -> None:
        print("Starting the handle register request function")
        client_name = self.conn.recv(CLIENT_NAME_SIZE)
        if not client_name:
            raise ConnectionError("Client didn't send his name.")
        print(f"trying to register with {client_name = }")

        if self.db.contains_name(client_name):
            print(f"Database already contains this {client_name = }.")
            self.response.code = REGISTER_FAILED_CODE
        else:
            client_id_bytes = uuid.uuid4().bytes
            print(f"Created client id: {client_id_bytes}")
            self.response.payload = client_id_bytes
            self.response.code = REGISTER_SUCCESS_CODE

    def receive_file(self, file_name: str, request):
        received = 0

        with open(file_name, 'wb') as f:
            while received < request.payload_size:
                next_chunk_size = min(BUFFER_SIZE, request.payload_size - received)
                print(f"next chunk size: {next_chunk_size}")
                data = self.conn.recv(next_chunk_size)
                print(f"We received data of size:{len(data)}.\n the data is: {data}")
                f.write(data)
                received += next_chunk_size
        print(f"File '{file_name}' received successfully!")

    def send_file(self, file_name):
        with open(file_name, 'rb') as f:
            while True:
                data = f.read(BUFFER_SIZE)
                print(f"We send data of size:{len(data)}.\n the data is: {data}")
                if not data:
                    break
                self.conn.sendall(data)
        self.conn.sendall(b'EOF')
        print(f"File '{file_name}' sent successfully!")
