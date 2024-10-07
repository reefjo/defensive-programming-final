import sqlite3
DATABASE_NAME = "db.defensive"
CREATE_CLIENTS_QUERY = '''
        CREATE TABLE IF NOT EXISTS clients (
            id BLOB PRIMARY KEY,  -- 16 bytes unique identifier for each client
            name TEXT NOT NULL,   -- ASCII string representing the username (null-terminated)
            public_key BLOB NOT NULL,  -- 160 bytes public key of the client
            last_seen DATETIME,   -- Date and time of last request from the client
            aes_key BLOB NOT NULL -- 256-bit AES key sent to the client
        )
        '''
CREATE_FILES_QUERY = '''
        CREATE TABLE IF NOT EXISTS files (
            id BLOB PRIMARY KEY,  -- 16 bytes unique identifier for each file
            name TEXT NOT NULL,    -- ASCII string representing the filename (null-terminated)
            path TEXT NOT NULL,    -- ASCII string representing the relative path where the file is stored (null-terminated)
            verified BOOLEAN NOT NULL  -- Boolean indicating if the checksum was successfully verified
        )
        '''
GET_CLIENTS_QUERY = '''
        select * from clients
'''

class Database:
    def __init__(self):
        self.db_name = DATABASE_NAME
        self.conn = sqlite3.connect(DATABASE_NAME)
        self.create_tables()

    def create_tables(self):
        # Create a cursor object to execute SQL commands
        cursor = self.conn.cursor()
        cursor.execute(CREATE_CLIENTS_QUERY)
        cursor.execute(CREATE_FILES_QUERY)
        self.conn.commit()
        cursor.close()

    def get_clients(self):
        print("trying to get all info from clients...")
        cursor = self.conn.cursor()
        rows = cursor.execute(GET_CLIENTS_QUERY)
        for row in rows:
            print(row)
        print("ended printing all rows from clients")

    def insert_into_clients(self, id, name, public_key, last_seen, aes_key):
        print("Trying to insert into clients...")
        cursor = self.conn.cursor()
        try:
            cursor.execute('''
                INSERT INTO clients (id, name, public_key, last_seen, aes_key)
                VALUES (?, ?, ?, ?, ?)
            ''', (id, name, public_key, last_seen, aes_key))
            self.conn.commit()  # Commit the changes
            print("Successfully inserted into clients.")
        except sqlite3.Error as e:
            print(f"Error occurred while inserting into clients: {e}")
        finally:
            cursor.close()  # Ensure the cursor is closed

    def close_connection(self):
        # Close the database connection
        self.conn.close()

















