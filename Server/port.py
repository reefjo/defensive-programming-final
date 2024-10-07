DEFAULT_PORT = 1256
PORT_FILE = "port.info"
def read_port_from_file() -> int:
    try:
        with open(PORT_FILE, "r") as file:
            return int(file.read().strip())
    except FileNotFoundError:
        print(f"File {PORT_FILE} not found, using default port")
    except Exception as e:
        print(f"An unexpected error occurred: {e}, using default port")

    return DEFAULT_PORT