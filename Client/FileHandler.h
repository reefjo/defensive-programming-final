#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>
#include <tuple>
#include <boost/asio.hpp>
#define TRANSFER_FILE_NAME "transfer.info"

class FileHandler {
public:
    // Function to read server IP, port, client name, and file path from info.transfer file
    std::tuple<std::string, std::string, std::string, std::string> readTransferFile();

    // Function to send file to server using Boost Asio
    void sendFile(const std::string& fileName, boost::asio::ip::tcp::socket& socket);

    // Function to receive file from server using Boost Asio
    void receiveFile(const std::string& fileName, boost::asio::ip::tcp::socket& socket);

    // (Optional) If you plan to use the get_file_from_connection function, you can declare it here
    std::string get_file_from_connection();
};
void convertSizeToLittleEndian(uint32_t size, char* buffer);
void prepareSizeForTransmission(uint32_t size, char* buffer);


#endif // FILE_HANDLER_H
