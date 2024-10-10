import socket
import database
import selectors

from Server.file_handler import FileHandler


class Server:
    def __init__(self, host: str, port: int) -> None:
        self.host = host
        self.port = port
        self.database = database.Database()
        self.selector = selectors.DefaultSelector()  # Initialize the selector

    def accept_connection(self, server_socket):
        connection, client_address = server_socket.accept()
        print(f"Connected to {client_address}")
        connection.setblocking(False)
        # Register the new connection for read events
        self.selector.register(connection, selectors.EVENT_READ, self.handle_client)

    def handle_client(self, connection):
        fh = FileHandler(connection)
        print("Created a file handler")
        fh.receive_file("receive_here_server.txt")

        data = connection.recv(1024)
        if data:
            print(f"Received data: {data.decode('utf-8')}")
            # Send a message back to the client
            connection.sendall(b"Message received!")
        else:
            # Close the connection if no data is received
            print("Closing connection")
            self.selector.unregister(connection)
            connection.close()


    def run(self):
        # Set up the server socket
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.bind((self.host, self.port))
        server_socket.listen(1)
        server_socket.setblocking(False)  # Set non-blocking mode

        # Register the server socket to accept new connections
        self.selector.register(server_socket, selectors.EVENT_READ, self.accept_connection)

        print(f"Server listening on port {self.port}")

        try:
            while True:
                events = self.selector.select(timeout=None)  # Blocking call, waits for events
                for key, mask in events:
                    callback = key.data  # This will either be accept_connection or handle_client
                    callback(key.fileobj)  # Call the callback associated with the event
        except KeyboardInterrupt:
            print("Server shutting down.")
        finally:
            # Clean up
            self.selector.close()
            server_socket.close()


