#include <iostream>
#include "Client.h"



int main(int argc, char* argv[]) {
	try {
		Client client;
		client.start();
	}
	catch (std::exception& e) {
		std::cerr << "Exception caught at main: " << e.what() << std::endl;
	}
	return 0;


}
/*
void hexify(const unsigned char* buffer, unsigned int length)
{
	std::ios::fmtflags f(std::cout.flags());
	std::cout << std::hex;
	for (size_t i = 0; i < length; i++)
		std::cout << std::setfill('0') << std::setw(2) << (0xFF & buffer[i]) << (((i + 1) % 16 == 0) ? "\n" : " ");
	std::cout << std::endl;
	std::cout.flags(f);
}
int main2() {


	// plain text (could be binary data as well)
	unsigned char plain[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
	std::cout << "plain:" << std::endl;
	hexify(plain, sizeof(plain));		// print binary data nicely

	// 1. Create an RSA decryptor. this is done here to generate a new private/public key pair
	RSAPrivateWrapper rsapriv;

	// 2. get the public key
	std::string pubkey = rsapriv.getPublicKey();	// you can get it as std::string ...
	std::cout << "pubkey: " << pubkey << std::endl;
	std::cout << "pubkey length: " << pubkey.length() << std::endl;
	std::string privkey = rsapriv.getPrivateKey();
	std::string pukey = rsapriv.getPublicKey();
	std::cout << "private key + length: " << privkey << "length: " << privkey.length() << std::endl;
	std::cout << "public key + length: " << pubkey << "length: " << pubkey.length() << std::endl;

	RSAPublicWrapper pub_wrapper(pubkey);
	std::string ciphered = pub_wrapper.encrypt((const char*)plain, sizeof(plain));
	std::cout << "encoded text:" << std::endl;
	hexify((unsigned char*)ciphered.c_str(), ciphered.length());	// print binary data nicely
	std::string decrypted = rsapriv.decrypt(ciphered);
	std::cout << "decoded text: " << decrypted << std::endl;
	hexify((unsigned char*)decrypted.c_str(), decrypted.length());	// print binary data nicely

	return 0;
	





}
*/