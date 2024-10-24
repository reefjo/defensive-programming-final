from port import read_port_from_file
from server import Server
HOST = 'localhost'


def main():
    server = Server(HOST, read_port_from_file())
    server.run()

if __name__ == '__main__':
     main()
