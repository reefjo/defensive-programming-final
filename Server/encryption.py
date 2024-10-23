from Crypto.Cipher import PKCS1_OAEP, AES
from Crypto.Random import get_random_bytes
from Crypto.PublicKey import RSA
from Crypto.Util import Padding
from protocol_constants import  AES_KEY_SIZE



def generate_aes_key() -> bytes:
    aes_key = get_random_bytes(AES_KEY_SIZE)
    return aes_key

def encrypt_aes_key(aes_key : bytes, public_key: bytes) -> bytes:
    # import the public key
    rsa_key = RSA.import_key(public_key)
    cipher_rsa = PKCS1_OAEP.new(rsa_key)
    encrypted_aes_key = cipher_rsa.encrypt(aes_key)
    print("Successfully encrypted aes key with the public RSA key")
    return encrypted_aes_key

def decrypt_aes_data(encrypted_data: bytes, aes_key: bytes) -> bytes:
    # Use zero IV to match C++ implementation
    iv = bytes(16)  # should be a constant somewhere?
    cipher = AES.new(aes_key, AES.MODE_CBC, iv=iv)

    # Decrypt the data
    print("Trying to decrypt the data, unpadding...")
    print(f"Length of encrypted data from file: {len(encrypted_data)}")
    decrypted_data = Padding.unpad(cipher.decrypt(encrypted_data), AES.block_size)
    print(f"Successfully decrypted the data: {decrypted_data = }")
    return decrypted_data


