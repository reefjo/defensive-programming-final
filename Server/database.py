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
DROP_CLIENTS_QUERY = "DROP TABLE IF EXISTS clients"
DROP_FILES_QUERY = "DROP TABLE IF EXISTS files"


class Database:
    def __init__(self):
        self.db_name = DATABASE_NAME
        self.conn = sqlite3.connect(DATABASE_NAME)
        self.create_tables()

    def create_tables(self):
        # Create a cursor object to execute SQL commands
        cursor = self.conn.cursor()
        cursor.execute(DROP_CLIENTS_QUERY)
        cursor.execute(DROP_FILES_QUERY)
        cursor.execute(CREATE_CLIENTS_QUERY)
        cursor.execute(CREATE_FILES_QUERY)
        self.conn.commit()
        cursor.close()

        # ------------ FILES OPERATIONS ------------

    def insert_into_files(self, client_id, file_name, path, verified=False) -> None:
        print("Trying to insert into files...")
        cursor = self.conn.cursor()
        try:
            cursor.execute('''
                INSERT INTO files (client_id, name, path, verified)
                VALUES (?, ?, ?, ?)
            ''', (client_id, file_name, path, verified))
            self.conn.commit()  # Commit the changes
            print("Successfully inserted into files.")
        except sqlite3.Error as e:
            print(f"Error occurred while inserting into files: {e}")
        finally:
            cursor.close()  # Ensure the cursor is closed

    def update_file_verification(self, client_id, file_name, verified):
        print("Trying to update file verification status...")
        cursor = self.conn.cursor()
        try:
            cursor.execute('''
                UPDATE files
                SET verified = ?
                WHERE client_id = ? AND name = ?
            ''', (verified, client_id, file_name))
            self.conn.commit()
            print("Successfully updated file verification status.")
        except sqlite3.Error as e:
            print(f"Error occurred while updating file verification status: {e}")
        finally:
            cursor.close()

    def get_files_by_client(self, client_id):
        print("Trying to get files for client...")
        cursor = self.conn.cursor()
        try:
            cursor.execute('''
                SELECT * FROM files WHERE client_id = ?
            ''', (client_id,))
            return cursor.fetchall()  # Return all files for the specified client
        except sqlite3.Error as e:
            print(f"Error occurred while fetching files for client: {e}")
        finally:
            cursor.close()

    # For debugging purposes
    def get_files(self):
        cursor = self.conn.cursor()
        try:
            rows = cursor.execute('''
        select * from files
''')
            return rows
        except sqlite3.Error as e:
            print(f"Error occurred while fetching files: {e}")
        finally:
            cursor.close()

        # ------------- CLIENTS OPERATIONS ------------

        # ------------- UPDATES ---------------

    def update_public_key(self, client_id, public_key):
        print("Trying to update public key...")
        cursor = self.conn.cursor()
        try:
            cursor.execute('''
            UPDATE clients
            SET public_key = ?
            WHERE id = ?
            ''', (public_key, client_id))
            self.conn.commit()
            print("Successfully updated client public key.")
        except sqlite3.Error as e:
            print(f"Error occurred while updating public key: {e}")
        finally:
            cursor.close()

    def update_last_seen(self, client_id, last_seen):
        print("Trying to update last seen...")
        cursor = self.conn.cursor()
        try:
            cursor.execute('''
            UPDATE clients
            SET last_seen = ?
            WHERE id = ?
            ''', (last_seen, client_id))
            self.conn.commit()
            print("Successfully updated client last seen.")
        except sqlite3.Error as e:
            print(f"Error occurred while updating client last seen: {e}")
        finally:
            cursor.close()

    def update_aes_key(self, client_id, aes_key):
        print("Trying to update aes key...")
        cursor = self.conn.cursor()
        try:
            cursor.execute('''
            UPDATE clients
            SET aes_key = ?
            WHERE id = ?
            ''', (aes_key, client_id))
            self.conn.commit()
            print("Successfully updated client aes key.")
        except sqlite3.Error as e:
            print(f"Error occurred while updating aes key: {e}")
        finally:
            cursor.close()

    # ------------ GET FUNCTIONS -------------

    # Get clients (for debugging purposes)
    def get_clients(self):
        print("trying to get all info from clients...")
        cursor = self.conn.cursor()
        rows = cursor.execute('''
        select * from clients
''')
        return rows

    def get_client_info(self, client_id):
        print("Trying to get client info...")
        cursor = self.conn.cursor()
        try:
            cursor.execute('''
            SELECT * FROM clients WHERE id = ?
            ''', (client_id,))
            return cursor.fetchone()  # Return the entire row as a tuple
        except sqlite3.Error as e:
            print(f"Error occurred while fetching client info: {e}")
        finally:
            cursor.close()

    def get_client_name(self, client_id):
        print("Trying to get client name...")
        cursor = self.conn.cursor()
        try:
            cursor.execute('''SELECT name FROM clients WHERE id = ?''', (client_id,))
            result = cursor.fetchone()
            return result[0] if result else None  # Return the name directly, or None if not found
        except sqlite3.Error as e:
            print(f"Error occurred while fetching client name: {e}")
        finally:
            cursor.close()

    def get_public_key(self, client_id):
        print("Trying to get client public key...")
        cursor = self.conn.cursor()
        try:
            cursor.execute('''SELECT public_key FROM clients WHERE id = ?''', (client_id,))
            result = cursor.fetchone()
            return result[0] if result else None  # Return the public key directly, or None if not found
        except sqlite3.Error as e:
            print(f"Error occurred while fetching client public key: {e}")
        finally:
            cursor.close()

    def get_last_seen(self, client_id):
        print("Trying to get client last seen...")
        cursor = self.conn.cursor()
        try:
            cursor.execute('''SELECT last_seen FROM clients WHERE id = ?''', (client_id,))
            result = cursor.fetchone()
            return result[0] if result else None  # Return the last seen time directly, or None if not found
        except sqlite3.Error as e:
            print(f"Error occurred while fetching client last seen: {e}")
        finally:
            cursor.close()

    def get_aes_key(self, client_id):
        print("Trying to get client aes key...")
        cursor = self.conn.cursor()
        try:
            cursor.execute('''SELECT aes_key FROM clients WHERE id = ?''', (client_id,))
            result = cursor.fetchone()
            return result[0] if result else None  # Return the AES key directly, or None if not found
        except sqlite3.Error as e:
            print(f"Error occurred while fetching client aes key: {e}")
        finally:
            cursor.close()

    # ----------- INSERT ---------
    def insert_into_clients(self, id, name, public_key=None, last_seen=None, aes_key=None) -> None:
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

    def contains_name(self, name: str) -> bool:
        cursor = self.conn.cursor()
        try:
            # Check if the client name exists in the clients table
            cursor.execute('''
                SELECT 1 FROM clients WHERE name = ?
            ''', (name,))

            # Fetch one result
            result = cursor.fetchone()

            # If a result is found, return True, otherwise False
            if result:
                return True
            else:
                return False
        except sqlite3.Error as e:
            print(f"Error occurred while checking for client name: {e}")
            return False
        finally:
            cursor.close()

    def close_connection(self):
        # Close the database connection
        self.conn.close()
