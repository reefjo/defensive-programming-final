import os
import struct
from pathlib import Path

from cksum import memcrc
from encryption import generate_aes_key, encrypt_aes_key, decrypt_aes_data
from datetime import datetime

from request import RequestHeader
from protocol_constants import (
    REGISTER_CODE, SEND_FILE_CODE, CLIENT_NAME_SIZE,
    REGISTER_SUCCESS_CODE, REGISTER_FAILED_CODE, SEND_KEY_CODE, SEND_KEY_REQUEST_FORMAT, SEND_KEY_PAYLOAD_SIZE,
    GENERAL_FAIL_CODE, RECEIVED_KEY_SUCCESS_CODE, SEND_FILE_REQUEST_FORMAT,
    FILE_PAYLOAD_SIZE_WITHOUT_DATA, RECEIVED_FILE_SUCCESS_CODE, INVALID_CRC_CODE, VALID_CRC_CODE,
    INVALID_CRC_FINAL_CODE, CLIENT_ID_SIZE, SERVER_ACK_CODE, FILE_NAME_SIZE, LOGIN_CODE, LOGIN_FAIL_CODE,
    LOGIN_SUCCESS_CODE, REQUEST_HEADER_SIZE, REQUEST_HEADER_FORMAT, CLIENT_FOLDERS_NAME,
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
        try:
            self.parse_request_header()
            info = self.db.get_client_info(self.request_header.client_id)
            print(f"Client info: {info}")
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

            current_time = datetime.now().strftime('%Y-%m-%d %H:%M:%S')  # Format current time
            self.db.update_last_seen(self.request_header.client_id, current_time)

            if self.request_header.code != SEND_FILE_CODE or self.response.payload:
                # Send a response if it's not a file upload or if there's a payload
                self.response.send_response(self.conn)

        except ConnectionError as e:
            # Handle the case where the client has disconnected
            print("Client disconnected")
            raise e

        except Exception as e:
            self.response.code = GENERAL_FAIL_CODE
            self.response.payload = b''  # Clear payload in case of failure
            self.response.send_response(self.conn)
            raise e



    def parse_request_header(self):
        """
        Parses the incoming request header to extract client ID, version, request code, and payload size.
        Raises an error if the header is incomplete (less data than expected).
        """
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

    def handle_register_request(self,) -> None:

        """
        Handles a client registration request by generating a new client ID, storing it in the database,
        and creating a dedicated folder for the client's files.
        A client is considered "registered" if: received ID before AND exchanged keys.
        This way we avoid edge cases which the protocol doesn't handle well.
        """

        print("Starting the handle register request function")
        client_name = self.get_data(CLIENT_NAME_SIZE)
        print(f"{client_name = }")

        public = self.db.get_aes_key(self.request_header.client_id)
        if public:
            print("This id is already registered and exchanged keys!")
            self.response.code = REGISTER_FAILED_CODE
            return

        # Generate new id, and store it inside database for this client id
        self.request_header.client_id = uuid.uuid4().bytes

        self.db.insert_into_clients(self.request_header.client_id, client_name)
        print(f"Register successful! id =  {self.request_header.client_id}")

        # Create a folder for that client's files using the client_id
        client_folder_path = self.get_client_folder()
        os.makedirs(client_folder_path, exist_ok=True)  # Create the folder if it doesn't exist
        print(f"Created folder for client: {self.request_header.client_id}")  # Log the folder creation

        self.response.payload = self.request_header.client_id
        self.response.code = REGISTER_SUCCESS_CODE

    def handle_login_request(self):
        """
        Handles a client login request by checking if the client has previously exchanged keys.
        If the login is successful, generates and stores a new AES key.
        """
        client_name = self.get_data(CLIENT_NAME_SIZE)

        public_key = self.db.get_public_key(self.request_header.client_id)
        if not public_key:
            # This client never exchanged keys : login failed
            print("This client never exchanged keys! can't login")
            self.response.code = LOGIN_FAIL_CODE
            return

        # Generate key and store in database
        aes_key = self.generate_and_store_aes_key()

        self.load_encrypted_key_and_id(public_key, aes_key)

        print(f"Client login successfully: {client_name}")

        self.response.code = LOGIN_SUCCESS_CODE

    def handle_crc_codes(self):
        """
        Handles CRC-related requests (valid, invalid, invalid_final CRC codes).
        Updates the database with the file verification status if VALID CRC.
        """
        self.validate_payload_size(FILE_NAME_SIZE)
        file_name = self.get_data(FILE_NAME_SIZE)

        if self.request_header.code == VALID_CRC_CODE:
            # change file to valid(True) inside database
            file_name_stripped = self.strip_file_name(file_name)
            self.db.update_file_verification(self.request_header.client_id, file_name_stripped, True)

        # send back acknowledgement code + client id
        self.response.payload = struct.pack('<%ds' % CLIENT_ID_SIZE, self.request_header.client_id)

        self.response.code = SERVER_ACK_CODE  # Acknowledge we received the request
        print("sending ack back to client")

    def handle_send_key_request(self):
        """
        Handles key exchange requests by receiving the client's public key,
        storing it in the database, and sending back the encrypted AES key.
        """
        data = self.conn.recv(self.request_header.payload_size)
        client_name, public_key = struct.unpack(SEND_KEY_REQUEST_FORMAT, data)

        # Load public key into the database
        self.db.update_public_key(self.request_header.client_id, public_key)

        # update response code and payload as successful handling, and send new aes key
        aes_key = self.generate_and_store_aes_key()

        self.load_encrypted_key_and_id(public_key, aes_key)
        self.response.code = RECEIVED_KEY_SUCCESS_CODE
        print("Encrypted AES key sent to client successfully")

    def generate_and_store_aes_key(self):
        """
        Generates a new AES key, stores it in the database for the current client id, and returns it.
        """
        aes_key = generate_aes_key()
        self.db.update_aes_key(self.request_header.client_id, aes_key)
        return aes_key

    def load_encrypted_key_and_id(self, public_key, aes_key ):  # useful for server responses: 1602 and 1605
        """
        Encrypts the AES key using the client's public key and loads it along with the client ID
        into the response payload.
        """
        encrypted_aes_key = encrypt_aes_key(aes_key, public_key)

        client_id = self.request_header.client_id
        self.response.payload = struct.pack('<%ds%ds' % (len(client_id), len(encrypted_aes_key)), client_id, encrypted_aes_key)

    def handle_send_file_request(self):
        """
        Handles file transfer from the client. It stores the encrypted data, decrypts the file
        when the last packet is received, and validates the checksum.
        """
        aes_key = self.db.get_aes_key(self.request_header.client_id)
        data = self.conn.recv(self.request_header.payload_size)

        file_data_len = self.request_header.payload_size - FILE_PAYLOAD_SIZE_WITHOUT_DATA
        structure_with_data = SEND_FILE_REQUEST_FORMAT + f"{file_data_len}s"

        content_size, orig_file_size, packet_number, total_packets, file_name, encrypted_data = struct.unpack(
            structure_with_data, data)

        print(f"Received {content_size = }, {orig_file_size = }, {packet_number = }, {total_packets = }")

        file_name_stripped = self.strip_file_name(file_name)
        client_folder_path = self.get_client_folder()
        file_path = os.path.join(client_folder_path, file_name_stripped)

        # Check if the file path is safe
        if not is_safe_file_path(file_path, client_folder_path):
            raise Exception("Dangerous file path detected!")

        mode = "wb" if packet_number == 1 else "ab"
        with open(file_path, mode) as f:
            f.write(encrypted_data)
        if packet_number == 1:  # Update database for the first packet
            self.db.insert_into_files(self.request_header.client_id, file_name_stripped, file_path)

        if packet_number == total_packets:  # it was the last packet, decrypt the file
            encrypted_file_data = ""
            with open(file_path, 'rb') as f:
                encrypted_file_data = f.read()

            decrypted_data = decrypt_aes_data(encrypted_file_data, aes_key)
            checksum = memcrc(decrypted_data)

            # Write decrypted data to file
            with open(file_path, 'wb') as f:  # open this file again and write the decrypted data
                f.write(decrypted_data)

            print(f"Successfully decrypted and wrote file: {file_name_stripped}")

            # Build the response payload with client ID, file size, and checksum
            self.response.payload = struct.pack('<%dsI%dsI' % (len(self.request_header.client_id), len(file_name)),
                                                self.request_header.client_id, len(encrypted_file_data), file_name, checksum)
            self.response.code = RECEIVED_FILE_SUCCESS_CODE

    def validate_payload_size(self, expected_size):
        """
        Validates the payload size received against the expected size.
        Raises an exception if the size is incorrect.
        """
        if self.request_header.payload_size != expected_size:
            raise ValueError(f"Invalid payload size. Expected {expected_size} bytes but got {self.request_header.payload_size} bytes")

    def get_data(self, size):
        """
        Helper function to receive 'size' bytes of data from the client connection.\
        Raises error if client disconnected
        """
        data = self.conn.recv(size)
        if not data:  # client disconnected
            raise ConnectionError(f"Couldn't fetch data : client disconnected, expected {size} bytes")
        return data

    def get_client_folder(self) -> str:
        """
        Returns the specific client's folder path, where files will be stored.
        """
        client_folder = os.path.join(str(CLIENT_FOLDERS_NAME), self.request_header.client_id.hex())
        return client_folder

    def strip_file_name(self, file_name):
        return file_name.rstrip(b'\x00').decode('utf-8')


def is_safe_file_path(file_path: str, folder_path: str) -> bool:
    """
    Checks if the given file path is safe, ensuring it resides within the specified folder
    and does not contain dangerous characters.
    """
    resolved_file_path = Path(file_path).resolve()
    resolved_folder_path = Path(folder_path).resolve()

    # Check if the file path starts with the folder path
    if not str(resolved_file_path).startswith(str(resolved_folder_path)):
        return False  # Path traversal detected

    # Check for dangerous characters in the file name
    dangerous_characters = ['/', '\\', ':', '*', '?', '"', '<', '>', '|']
    file_name = resolved_file_path.name
    if any(char in dangerous_characters for char in file_name):
        return False  # Dangerous characters found

    return True  # Safe file path
