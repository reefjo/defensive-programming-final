import socket

DEFAULT_PORT = 1256
PORT_FILE = "port.info"
# This is a comment i  want to see if changes somethuing
def get_port_from_file() -> int:
    try:
        with open(PORT_FILE, "r") as file:
            return int(file.read().strip())
    except FileNotFoundError:
        print(f"File {PORT_FILE} not found, using default port")
    except Exception as e:
        print(f"An unexpected error occurred: {e}, using default port")

    return DEFAULT_PORT





def start_server():
    print("trying to run")
    port = get_port_from_file()

    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(('localhost', port))
    server_socket.listen(1)
    print(f"Server listening on port {port}")

    connection, client_address = server_socket.accept()
    with connection:
        print(f"Connected to {client_address}")
        while True:
            data = connection.recv(1024)
            if not data:
                break
            print(f"Received data: {data.decode('utf-8')}")
            # Send a message back to the client
            connection.sendall(b"Message received!")

    # Close the server socket after the loop
    server_socket.close()





if __name__ == '__main__':
    start_server()


