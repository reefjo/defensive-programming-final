import socket
from request import Request
from protocol_constants import (
    REGISTER_CODE, SEND_FILE_CODE, CLIENT_NAME_SIZE, BUFFER_SIZE,
    REGISTER_SUCCESS_CODE, REGISTER_FAILED_CODE
)
from database import Database
from response import Response
import uuid


class RequestHandler:
    def __init__(self, conn, db: Database):
        self.conn = conn
        self.db = db
        self.request = Request()
        self.response = Response()

    def handle_request(self):
        print("Request received from client")
        self.request.parse_from_socket(self.conn)

        if self.request.code == REGISTER_CODE:
            self.handle_register_request(self.request)
        elif self.request.code == SEND_FILE_CODE:
            self.receive_file("receive_here_server.txt")
        # send the response
        print(f"After parsing request, trying to send response")
        self.response.send_response(self.conn)

    def handle_register_request(self, request: Request) -> None:
        print("Starting the handle register request function")
        client_name = self.conn.recv(CLIENT_NAME_SIZE)
        if not client_name:
            raise Exception
        print(f"trying to register with {client_name = }")

        if not self.db.contains_name(client_name):
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
