import sqlite3

DATABASE_NAME = "db.defensive"
CREATE_CLIENTS_QUERY = '''
        CREATE TABLE IF NOT EXISTS clients (
            id BLOB PRIMARY KEY,  -- 16 bytes unique identifier for each client
            name TEXT NOT NULL,   -- ASCII string representing the username (null-terminated)
            public_key BLOB ,  -- 160 bytes public key of the client
            last_seen DATETIME,   -- Date and time of last request from the client
            aes_key BLOB  -- 256-bit AES key sent to the client
        )
        '''
CREATE_FILES_QUERY = '''
        CREATE TABLE IF NOT EXISTS files (
            client_id BLOB,  -- 16 bytes unique identifier for the client (foreign key)
            name TEXT NOT NULL,    -- ASCII string representing the filename (null-terminated)
            path TEXT NOT NULL,    -- ASCII string representing the relative path where the file is stored (null-terminated)
            verified BOOLEAN NOT NULL,  -- Boolean indicating if the checksum was successfully verified
            PRIMARY KEY (client_id, name),  -- Composite primary key
            FOREIGN KEY (client_id) REFERENCES clients(id) ON DELETE CASCADE
        )
        '''
GET_CLIENTS_QUERY = '''
        select * from clients
'''

# SQL queries to drop the tables if they already exist (good for debugging)
DROP_CLIENTS_QUERY = "DROP TABLE IF EXISTS clients"
DROP_FILES_QUERY = "DROP TABLE IF EXISTS files"


class Database:
    # Initializes the database connection and ensures tables are created.
    def __init__(self):
        self.db_name = DATABASE_NAME
        self.conn = sqlite3.connect(DATABASE_NAME)
        self.create_tables()

    # Creates the 'clients' and 'files' tables in the database if they don't exist.
    def create_tables(self):
        cursor = self.conn.cursor()
        #cursor.execute(DROP_CLIENTS_QUERY)
        #cursor.execute(DROP_FILES_QUERY)
        cursor.execute(CREATE_CLIENTS_QUERY)
        cursor.execute(CREATE_FILES_QUERY)
        self.conn.commit()
        cursor.close()

        # ------------ FILES INSERTIONS ------------

    def insert_into_files(self, client_id, file_name, path, verified=False) -> None:
        cursor = self.conn.cursor()
        cursor.execute('''
            INSERT OR REPLACE INTO files (client_id, name, path, verified)
            VALUES (?, ?, ?, ?)
        ''', (client_id, file_name, path, verified))
        self.conn.commit()
        cursor.close()

        # ------------ FILES UPDATES ------------
        # Functions to update a given parameter based on client id + file name

    def update_file_verification(self, client_id, file_name, verified):
        cursor = self.conn.cursor()
        cursor.execute('''
            UPDATE files
            SET verified = ?
            WHERE client_id = ? AND name = ?
        ''', (verified, client_id, file_name))
        self.conn.commit()
        cursor.close()

    def get_files_by_client(self, client_id):
        cursor = self.conn.cursor()
        cursor.execute('''
            SELECT * FROM files WHERE client_id = ?
        ''', (client_id,))
        return cursor.fetchall()

    # For debugging purposes
    def get_files(self):
        cursor = self.conn.cursor()
        rows = cursor.execute('''
            SELECT * FROM files
        ''')
        return rows

    # -------------- CLIENTS ------------

    # -------------- UPDATES ---------------
    # Functions to update a given parameter based on client id

    def update_public_key(self, client_id, public_key):
        cursor = self.conn.cursor()
        cursor.execute('''
            UPDATE clients
            SET public_key = ?
            WHERE id = ?
        ''', (public_key, client_id))
        self.conn.commit()
        cursor.close()

    def update_last_seen(self, client_id, last_seen):
        cursor = self.conn.cursor()
        cursor.execute('''
            UPDATE clients
            SET last_seen = ?
            WHERE id = ?
        ''', (last_seen, client_id))
        self.conn.commit()
        cursor.close()

    def update_aes_key(self, client_id, aes_key):
        cursor = self.conn.cursor()
        cursor.execute('''
            UPDATE clients
            SET aes_key = ?
            WHERE id = ?
        ''', (aes_key, client_id))
        self.conn.commit()
        cursor.close()

    # ------------ GET FUNCTIONS -------------
    # Functions to get a specific parameter from given client id. returns None client id if not found

    # This is for debugging purposes used by the server console outputs
    def get_client_info(self, client_id):
        cursor = self.conn.cursor()
        cursor.execute('''
            SELECT * FROM clients WHERE id = ?
        ''', (client_id,))
        return cursor.fetchone()


    def get_client_name(self, client_id):
        cursor = self.conn.cursor()
        cursor.execute('''SELECT name FROM clients WHERE id = ?''', (client_id,))
        result = cursor.fetchone()
        return result[0] if result else None

    def get_public_key(self, client_id):
        cursor = self.conn.cursor()
        cursor.execute('''SELECT public_key FROM clients WHERE id = ?''', (client_id,))
        result = cursor.fetchone()
        return result[0] if result else None


    def get_last_seen(self, client_id):
        cursor = self.conn.cursor()
        cursor.execute('''SELECT last_seen FROM clients WHERE id = ?''', (client_id,))
        result = cursor.fetchone()
        return result[0] if result else None

    def get_aes_key(self, client_id):
        cursor = self.conn.cursor()
        cursor.execute('''SELECT aes_key FROM clients WHERE id = ?''', (client_id,))
        result = cursor.fetchone()
        return result[0] if result else None

    # ----------- INSERT ---------
    def insert_into_clients(self, id, name, public_key=None, last_seen=None, aes_key=None) -> None:
        cursor = self.conn.cursor()
        cursor.execute('''
            INSERT INTO clients (id, name, public_key, last_seen, aes_key)
            VALUES (?, ?, ?, ?, ?)
        ''', (id, name, public_key, last_seen, aes_key))
        self.conn.commit()
        cursor.close()


    def close_connection(self):
        # Close the database connection
        self.conn.close()
