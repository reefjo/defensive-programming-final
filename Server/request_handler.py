import socket
BUFFER_SIZE = 1024

class RequestHandler:
    def __init__(self, conn):
        self.conn = conn
    def receive_request(self):
        client_id = self.conn.recv(16)
        client_version = self.conn.recv(1)
        code = self.conn.recv(2)
        # First, receive the first 4 bytes (file size)
        payload_size = self.conn.recv(4)
        if code == 828:
            self.receive_file("receive_here_server.txt")

    def receive_file(self, file_name):
        # Receive the client id ( 16 bytes)
        client_id = self.conn.recv(16)
        client_version = self.conn.recv(1)
        code = self.conn.recv(2)
        # First, receive the first 4 bytes (file size)
        file_size_bytes = self.conn.recv(4)
        if len(file_size_bytes) < 4:
            print("Failed to receive file size.")
            return

        # Convert the file size bytes to an integer (little-endian)
        file_size = int.from_bytes(file_size_bytes, 'little')
        print(f"File size: {file_size} bytes")

        received = 0

        with open(file_name, 'wb') as f:
            while received < file_size:
                next_chunk_size = min(BUFFER_SIZE, file_size - received)
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
