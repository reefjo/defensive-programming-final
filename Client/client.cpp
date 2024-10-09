#include <iostream>
#include <fstream> 
#include <string>    // for std::string, std::getline, std::stoi
#include <boost/asio.hpp>
#include "FileHelper.h"

using boost::asio::ip::tcp;

void test_messages(int client_id) { // Accept client_id as a parameter
    try {
        boost::asio::io_service io_service;
        tcp::socket socket(io_service);
        auto [server_ip, server_port, client_name, file_path] = readTransferFile();

        // Resolve the server address and port
        tcp::resolver resolver(io_service);
        auto endpoint = resolver.resolve({ server_ip, server_port });
        boost::asio::connect(socket, endpoint);

        std::cout << "Client " << client_id << " (" << client_name << ") connected. Waiting to terminate...\n";

        // Send the first message
        const std::string message = "Hello from C++ Client " + std::to_string(client_id) + "!";
        boost::asio::write(socket, boost::asio::buffer(message));

        // Receive and print the response
        char reply[1024];
        size_t reply_length = socket.read_some(boost::asio::buffer(reply));
        std::cout << "Reply from server: ";
        std::cout.write(reply, reply_length);
        std::cout << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(10));  // Sleep for 15 seconds

        // Send the second message
        const std::string message2 = "Hello22 from C++ Client " + std::to_string(client_id) + "!";
        boost::asio::write(socket, boost::asio::buffer(message2));

        // Receive and print the second response
        reply_length = socket.read_some(boost::asio::buffer(reply));
        std::cout << "Reply from server: ";
        std::cout.write(reply, reply_length);
        std::cout << std::endl;

        // Close the socket
        socket.close();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}


void test_clients() {
    const int NUM_CLIENTS = 5; // Number of clients to simulate
    std::vector<std::thread> threads;

    for (int i = 0; i < NUM_CLIENTS; ++i) {
        threads.emplace_back(test_messages, i + 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Stagger client connections
    }

    // Wait for all client threads to finish
    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << "Finished testing multiple clients." << std::endl; //cmnt for .igitnore
}
int main() {
    // test_messages();
    test_clients();


    return 0;
}
