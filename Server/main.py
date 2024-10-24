import sqlite3
import socket
from port import read_port_from_file
from server import Server
from database import Database
from typing import List
HOST = 'localhost'


def test_database():
    db = Database()

    # Example usage: Insert a new client
    client_id = b'someuniqueid123457'  # Example 16-byte unique ID as a bytes object
    client_name = 'user2'  # ASCII string representing the username
    client_id2 = b'someuniqueid123999'  # Example 16-byte unique ID as a bytes object
    client_name2 = 'user99'  # ASCII string representing the username

    client_public_key = b'somepublickey1234567890'  # Example public key as a bytes object
    client_last_seen = '2024-10-07 12:00:00'  # Example datetime string
    client_aes_key = b'someaeskey12345678901234567890'  # Example AES key as a bytes object
    another_aes_key = b'someaeskey12345678901234569999'

    db.insert_into_clients(client_id, client_name, client_public_key, client_last_seen, client_aes_key)
    clients = db.get_clients()
    for row in clients:
        print(row)
    aes_key = db.get_aes_key(b'}\xfa\x1f\x93-\xd1H\x01\x89\x02o\x03\xaf\xca|\xfc')
    print(aes_key)
    # Close the database connection when done
    db.close_connection()


    # Close the database connection when done
    db.close_connection()


def main():
    server = Server(HOST, read_port_from_file())
    server.run()

if __name__ == '__main__':
     main()
     #test_database()
