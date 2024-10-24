import socket
import struct


from cksum import memcrc
from encryption import generate_aes_key, encrypt_aes_key, decrypt_aes_data

from request import RequestHeader
from protocol_constants import (
    REGISTER_CODE, SEND_FILE_CODE, CLIENT_NAME_SIZE, BUFFER_SIZE,
    REGISTER_SUCCESS_CODE, REGISTER_FAILED_CODE, SEND_KEY_CODE, SEND_KEY_REQUEST_FORMAT, SEND_KEY_PAYLOAD_SIZE,
    AES_KEY_SIZE, KEY_SIZE, GENERAL_FAIL_CODE, RECEIVED_KEY_SUCCESS_CODE, SEND_FILE_REQUEST_FORMAT,
    FILE_PAYLOAD_SIZE_WITHOUT_DATA, RECEIVED_FILE_SUCCESS_CODE, INVALID_CRC_CODE, VALID_CRC_CODE,
    INVALID_CRC_FINAL_CODE, CLIENT_ID_SIZE, SERVER_ACK_CODE, FILE_NAME_SIZE, LOGIN_CODE, LOGIN_FAIL_CODE,
    LOGIN_SUCCESS_CODE, REQUEST_HEADER_SIZE, REQUEST_HEADER_FORMAT,
)
from database import Database
from response import Response
import uuid


class RequestHandler:
    def __init__(self, conn, db: Database, id_to_aes_key : dict, id_to_public_key):
        self.conn = conn
        self.db = db
        self.request_header = RequestHeader()
        self.response = Response()
        self.id_to_aes_key = id_to_aes_key
        self.id_to_public_key = id_to_public_key

    def handle_request(self):
        try:
            self.parse_request_header()
            print("Request received from client")
            # if an error has occurred, put general code fail

            code = self.request_header.code
            if code == REGISTER_CODE:
                self.handle_register_request()
            elif code == SEND_FILE_CODE:
                self.handle_send_file_request()
            elif code == SEND_KEY_CODE:
                self.handle_send_key_request()
            elif code == INVALID_CRC_CODE or code == VALID_CRC_CODE or code == INVALID_CRC_FINAL_CODE:
                self.handle_crc_codes()
            elif code == LOGIN_CODE:
                self.handle_login_request()
            else:
                raise Exception("Unknown request code")

            if self.request_header.code != SEND_FILE_CODE or self.response.payload:
                # Server sends a response in either condition:
                # 1. It's not a send file request
                # 2. It's a send file request, and we just received the last package (thus payload is not None)
                self.response.send_response(self.conn)
        except ConnectionError as e:
            # Handle the case where the client has disconnected
            print("Client disconnected")
            raise e  # Reraise to allow the server to handle this

        except Exception as e:
            print(f"Error handling request: {str(e)}")
            # If an error occurs, set the response code to GENERAL_FAIL_CODE
            self.response.code = GENERAL_FAIL_CODE
            self.response.payload = b''  # Clear payload in case of failure
            self.response.send_response(self.conn)
            raise e  # re raise the exception to handle it in server



    def parse_request_header(self):
        # Read the entire header in one go using struct

        data = self.get_data(REQUEST_HEADER_SIZE)

        if len(data) < REQUEST_HEADER_SIZE:
            raise ConnectionError(f"Insufficient data received from client (less than expected): {data = }")

        # Unpack header data using struct
        (self.request_header.client_id, self.request_header.client_version, self.request_header.code,
         self.request_header.payload_size) = struct.unpack(REQUEST_HEADER_FORMAT, data)

        print(
            f"Parsed from client: client_id={self.request_header.client_id},"
            f" client_version={self.request_header.client_version}, code={self.request_header.code},"
            f" payload_size={self.request_header.payload_size}")

    def handle_login_request(self):
        # if client is in hash set of clients , accept. otherwise, send fail
        client_name = self.get_data(CLIENT_NAME_SIZE)

        if self.request_header.client_id not in self.id_to_aes_key:
            print(f"Failed to login because {client_name} is not registered ")
            self.response.code = LOGIN_FAIL_CODE
            return

        print(f"Client login successfully: {client_name}")
        public_key = self.id_to_public_key[self.request_header.client_id]
        self.load_encrypted_key_and_id(public_key)
        self.response.code = LOGIN_SUCCESS_CODE

    def handle_crc_codes(self):
        # payload should be: client name (although we don't use it)
        self.validate_payload_size(CLIENT_NAME_SIZE)
        data = self.get_data(CLIENT_NAME_SIZE)  # fetch data from socket, don't use it

        # send back acknowledgement code + client id
        self.response.payload = struct.pack('<%ds' % CLIENT_ID_SIZE, self.request_header.client_id)

        self.response.code = SERVER_ACK_CODE  # Acknowledge we received the request
        print("sending ack back to client")

    def handle_send_key_request(self):
        # Receives the public key of the client and saves it, then saves the encrypted key back

        data = self.conn.recv(self.request_header.payload_size)
        client_name, public_key = struct.unpack(SEND_KEY_REQUEST_FORMAT, data)
        print(f"{len(client_name) = }, {len(public_key) = }")
        print(f"Client name received : {client_name}")
        print(f"Client public key received: {public_key}")
        print("Thank you for sending me the key! storing it ...")

        self.id_to_public_key[self.request_header.client_id] = public_key

        # update response code and payload as successful handling, and send new aes key
        self.load_encrypted_key_and_id(public_key)
        self.response.code = RECEIVED_KEY_SUCCESS_CODE
        print("Encrypted key sent to client, and stored id->aes_key.")

    def load_encrypted_key_and_id(self, public_key):  # useful for server responses: 1602 and 1605

        # Generate AES key
        aes_key = generate_aes_key()

        # Assign the key to this id
        self.id_to_aes_key[self.request_header.client_id] = aes_key

        encrypted_aes_key = encrypt_aes_key(aes_key, public_key)
        print(f"{encrypted_aes_key = }")


        client_id = self.request_header.client_id
        self.response.payload = struct.pack('<%ds%ds' % (len(client_id), len(encrypted_aes_key)), client_id, encrypted_aes_key)

    def handle_send_file_request(self):
        # store the encrypted data. in the end(last packet), take encrypted data out and put the decrypted data

        if self.request_header.client_id not in self.id_to_aes_key:
            raise Exception("Client has not registered, client id not found")

        data = self.conn.recv(self.request_header.payload_size)
        file_data_len = self.request_header.payload_size - FILE_PAYLOAD_SIZE_WITHOUT_DATA
        structure_with_data = SEND_FILE_REQUEST_FORMAT + f"{file_data_len}s"


        content_size, orig_file_size, packet_number, total_packets, file_name, encrypted_data = struct.unpack(
            structure_with_data, data)
        print(f"Received {content_size = }, {orig_file_size = }, {packet_number = }")
        print(f"{total_packets = }, {file_name = },\n {encrypted_data = }")
        print("Thank you for sending the file! trying to store it encrypted")
        print(f"The encrypted data length: {len(encrypted_data)}, should be {content_size}")

        file_name_stripped = file_name.rstrip(b'\x00').decode('utf-8')

        mode = "wb" if packet_number == 1 else "ab"
        with open(file_name_stripped, mode) as f:
            f.write(encrypted_data)
        if packet_number == total_packets:  # it was the last packet
            encrypted_file_data = ""
            with open(file_name_stripped, 'rb') as f:
                encrypted_file_data = f.read()
            aes_key = self.id_to_aes_key[self.request_header.client_id]

            decrypted_data = decrypt_aes_data(encrypted_file_data, aes_key)
            checksum = memcrc(decrypted_data)
            # Remove padding - use the original file size as reference

            # Write decrypted data to file
            with open(file_name_stripped, 'wb') as f:  # open this file again and write the decrypted data
                f.write(decrypted_data)

            print(f"Successfully decrypted and wrote file: {file_name_stripped}")
            print(f"{checksum = }")

            # Dynamically build the format (since file length is changing)
            self.response.payload = struct.pack('<%dsI%dsI' % (len(self.request_header.client_id), len(file_name)),
                                                self.request_header.client_id, len(encrypted_file_data), file_name, checksum)
            self.response.code = RECEIVED_FILE_SUCCESS_CODE


    def handle_register_request(self,) -> None:
        print("Starting the handle register request function")
        client_name = self.get_data(CLIENT_NAME_SIZE)

        print(f"trying to register with {client_name = }")

        if self.db.contains_name(client_name):
            print(f"Database already contains this {client_name = }.")
            self.response.code = REGISTER_FAILED_CODE
        else:
            client_id_bytes = uuid.uuid4().bytes
            print(f"Register successful! Created client id: {client_id_bytes}")
            self.response.payload = client_id_bytes
            self.response.code = REGISTER_SUCCESS_CODE

    def validate_payload_size(self, expected_size):
        if self.request_header.payload_size != expected_size:
            raise ValueError(f"Invalid payload size. Expected {expected_size} bytes but got {self.request_header.payload_size} bytes")

    def get_data(self, size):
        data = self.conn.recv(size)
        if not data:  # client disconnected
            raise ConnectionError(f"Couldn't fetch data : client disconnected, expected {size} bytes")
        return data

