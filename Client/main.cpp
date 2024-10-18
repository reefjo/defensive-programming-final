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