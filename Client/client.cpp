#include <iostream>
#include <fstream> 
#include <boost/asio.hpp>
using boost::asio::ip::tcp;
const std::string TRANSFER_FILE_NAME = "transfer.info";


// Function to read server IP, port, client name, and file path from info.transfer file
std::tuple<std::string, int, std::string, std::string> readTransferFile(const std::string filename) {
    std::ifstream transfer_file(filename);
    if (!transfer_file) {
        throw std::runtime_error("Couldn't read " + filename + " file");
    }
    // Read the lines from the file
    std::string server_ip_port, client_name, file_path;
    std::getline(transfer_file, server_ip_port);
    std::getline(transfer_file, client_name);
    std::getline(transfer_file, file_path);

    // Extract server IP and port
    std::string server_ip = server_ip_port.substr(0, server_ip_port.find(':'));
    std::string port_str = server_ip_port.substr(server_ip_port.find(':') + 1);
    int server_port = std::stoi(port_str);

    return { server_ip, server_port, client_name, file_path };
}


int main() {
    /*
    try {
        boost::asio::io_service io_service;
        tcp::socket socket(io_service);

        // Resolve the server address and port
        tcp::resolver resolver(io_service);
        auto endpoint = resolver.resolve({ "localhost", "65432" });
        boost::asio::connect(socket, endpoint);

        // Send the first message
        const std::string message = "Hello from C++ Client!";
        boost::asio::write(socket, boost::asio::buffer(message));

        // Receive and print the response
        char reply[1024];
        size_t reply_length = socket.read_some(boost::asio::buffer(reply));
        std::cout << "Reply from server: ";
        std::cout.write(reply, reply_length);
        std::cout << std::endl;

        // Send the second message
        const std::string message2 = "Hello22 from C++ Client!";
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
    */
    try {
        // Use the readTransferFile function to get the server IP, port, client name, and file path
        auto [server_ip, server_port, client_name, file_path] = readTransferFile(TRANSFER_FILE_NAME);

        // Continue with your application logic...
        std::cout << "Server IP: " << server_ip << "\n";
        std::cout << "Server Port: " << server_port << "\n";
        std::cout << "Client Name: " << client_name << "\n";
        std::cout << "File Path: " << file_path << "\n";
        // adding a comment to see if git changes this file or not

    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }


    return 0;
}
