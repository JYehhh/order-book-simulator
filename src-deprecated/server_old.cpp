#include <iostream>
#include "server.h"

#include <boost/asio.hpp>

#define STD_BUF_SIZE 1024
#define SERVER_PORT 5000

const boost::asio::ip::udp::endpoint SERVER_ENDPOINT
(
    boost::asio::ip::make_address("127.0.0.1"),
    SERVER_PORT
);

void Server() {
    // create context
    boost::asio::io_context ctx;

    // setup socket
    // context is linked to 
    boost::asio::ip::udp::socket socket(ctx, SERVER_ENDPOINT);

    std::vector<char> recv_buffer(STD_BUF_SIZE);

    auto recv_buffer = std::make_shared<std::vector<char>>(STD_BUF_SIZE);


    auto msg_handler = [&, recv_buffer](const boost::system::error_code& error, std::size_t bytes_transferred) {
        if (!error) {
            // Print the received message
            std::cout << "RCV: " << std::string(recv_buffer.data(), bytes_transferred) << "\n";
        } else {
            std::cerr << "ERR: " << error.message() << "\n";
        }

        // Listen for the next message
        socket.async_receive(
            boost::asio::buffer(recv_buffer), msg_handler);
    }

    socket.async_receive(
        boost::asio::buffer(recv_buffer), msg_handler);

    std::cout << "Server is listening on port " << SERVER_PORT << "...\n";
    
    // run is not an async call, it will block until everything in the context queue is done
    ctx.run();
}


int main(void) {
    std::cout << "Initialising Server..." << std::endl;
    Server();
    return 0;
}