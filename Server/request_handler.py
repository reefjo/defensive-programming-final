import socket
import struct
import Crypto
from Crypto.Cipher import PKCS1_OAEP, AES
from Crypto.Random import get_random_bytes
from Crypto.PublicKey import RSA
from Crypto.Util import Padding
from Crypto.Util.Padding import unpad

from request import RequestHeader
from protocol_constants import (
    REGISTER_CODE, SEND_FILE_CODE, CLIENT_NAME_SIZE, BUFFER_SIZE,
    REGISTER_SUCCESS_CODE, REGISTER_FAILED_CODE, SEND_KEY_CODE, SEND_KEY_REQUEST_STRUCTURE, SEND_KEY_PAYLOAD_SIZE,
    AES_KEY_SIZE, KEY_SIZE, GENERAL_FAIL_CODE, RECEIVED_KEY_SUCCESS_CODE, SEND_FILE_REQUEST_STRUCTURE,
    FILE_PAYLOAD_SIZE_WITHOUT_DATA
)
from database import Database
from response import Response
import uuid


class RequestHandler:
    def __init__(self, conn, db: Database, id_to_key : dict, registered_ids : set):
        self.conn = conn
        self.db = db
        self.request_header = RequestHeader()
        self.response = Response()
        self.id_to_key = id_to_key

    def handle_request(self):
        self.request_header.parse_from_socket(self.conn)
        print("Request received from client")


        if self.request_header.code == REGISTER_CODE:
            self.handle_register_request()
        elif self.request_header.code == SEND_FILE_CODE:
            self.handle_send_file_request()
        elif self.request_header.code == SEND_KEY_CODE:
            self.handle_send_key_request()
        else:
            print(f"Unknown request code: {self.request_header.code}")
            self.response.code = GENERAL_FAIL_CODE
        # send the response
        print(f"After parsing request, trying to send response")
        self.response.send_response(self.conn)

    def handle_send_key_request(self):
        # Receives the public key of the client and saves it, then saves the encrypted key back
        data = self.conn.recv(self.request_header.payload_size)
        client_name, public_key = struct.unpack(SEND_KEY_REQUEST_STRUCTURE, data)
        print(f"{len(client_name) = }, {len(public_key) = }")
        print(f"Client name received : {client_name}")
        print(f"Client public key received: {public_key}")
        print("Thank you for sending me the key! storing it ...(notreally)")

        self.id_to_key[self.request_header.client_id] = public_key


        self.response.code = RECEIVED_KEY_SUCCESS_CODE
        self.load_encrypted_key_and_id(public_key)
        print("Encrypted key sent to client, and stored id->aes_key.")

    def load_encrypted_key_and_id(self, public_key):  # useful for server responses: 1602 and 1605

        # import the public key
        rsa_key = RSA.import_key(public_key)

        # Generate AES key
        aes_key = get_random_bytes(AES_KEY_SIZE)

        # Assign the key to this id
        self.id_to_key[self.request_header.client_id] = aes_key

        # Encrypt the AES key with the client's public key
        cipher_rsa = PKCS1_OAEP.new(rsa_key)
        encrypted_aes_key = cipher_rsa.encrypt(aes_key)
        print(f"{encrypted_aes_key = }")

        # Dynamically pack
        client_id = self.request_header.client_id
        self.response.payload = struct.pack('<%ds%ds' % (len(client_id), len(encrypted_aes_key)), client_id, encrypted_aes_key)

    def handle_send_file_request(self):
        if self.request_header.client_id not in self.id_to_key:
            raise Exception("Client has not registered, client id not found")
        aes_key = self.id_to_key[self.request_header.client_id]

        data = self.conn.recv(self.request_header.payload_size)
        file_data_len = self.request_header.payload_size - FILE_PAYLOAD_SIZE_WITHOUT_DATA
        structure_with_data = SEND_FILE_REQUEST_STRUCTURE + f"{file_data_len}s"

        content_size, orig_file_size, packet_number, total_packets, file_name, encrypted_data = struct.unpack(
            structure_with_data, data)
        print(f"Received {content_size = }, {orig_file_size = }, {packet_number = }")
        print(f"{total_packets = }, {file_name = },\n {encrypted_data = }")
        print("Thank you for sending the file! trying to store it")
        print(f"The encrypted data length: {len(encrypted_data)}, should be {content_size}")

        file_name = file_name.rstrip(b'\x00').decode('utf-8')

        # Use zero IV to match C++ implementation
        iv = bytes(16)  # Create a zero IV
        cipher = AES.new(aes_key, AES.MODE_CBC, iv=iv)

        # Decrypt the data
        decrypted_data = Padding.unpad(cipher.decrypt(encrypted_data), AES.block_size)
        # Remove padding - use the original file size as reference

        # Write decrypted data to file
        with open(file_name, 'wb') as f:
            f.write(decrypted_data)

        print(f"Successfully decrypted and wrote file: {file_name}")



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
