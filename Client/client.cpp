#include <iostream>
#include <fstream> 
#include <string>    // for std::string, std::getline, std::stoi
#include <boost/asio.hpp>
#include "FileHandler.h"

using boost::asio::ip::tcp;


void test_sending() {
    try {
        boost::asio::io_service io_service;

        // Create socket and connect to server
        tcp::socket socket(io_service);
        FileHandler fileHandler;

        // Read connection info from transfer file
        auto [server_ip, server_port, client_name, file_path] = fileHandler.readTransferFile();

        // Resolve server address and port
        tcp::resolver resolver(io_service);
        auto endpoint = resolver.resolve({ server_ip, server_port });
        boost::asio::connect(socket, endpoint);

        std::cout << "Connected to server!" << std::endl;

        // Example: send and receive file
        std::string fileToSend = "client_to_send.txt";  // Modify with your file path
        std::string fileToReceive = "client_to_receive.txt";  // Modify with your desired file path

        // Send file to server
        fileHandler.sendFile(fileToSend, socket);

        // Receive file from server
        //fileHandler.receiveFile(fileToReceive, socket);

        // Close socket
        socket.close();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

}
int main() {
    // test_messages();
    test_sending();


    return 0;
}
