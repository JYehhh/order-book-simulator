#include <iostream>
#include <boost/asio.hpp>

#include <iostream>
#include <boost/asio.hpp>

class Client {
    public:
    Client(const std::string& ip, uint16_t port) : resolver_(context_), socket_(context_) {
        auto endpoints = resolver_.resolve(ip, std::to_string(port));
        boost::asio::connect(socket_, endpoints);
        std::cout << "Connected to server at " << ip << ":" << port << "\n";
    }

    void start_send() {
        std::cout << "Type messages to send (type 'exit' to quit):\n";
        std::string message;
        while (true) {
            std::cout << "> ";
            std::getline(std::cin, message);

            if (message == "exit") break;

            message += '\0';

            boost::asio::async_write(
                socket_, 
                boost::asio::buffer(message),
                [](boost::system::error_code ec, std::size_t) {
                    if (ec) {
                        std::cerr << "Send error: " << ec.message() << std::endl;
                    }
                }
            );

            std::cout << "Message sent: " << message << "\n";
        }

        socket_.close();
    }

    void start_rcv() {
        async_rcv();
        context_.run();
    }

    private:
    void async_rcv() {
        boost::asio::async_read_until(
            socket_, buffer_, '\0',
            [this](boost::system::error_code ec, std::size_t bytes_transferred) {
                if (!ec) {
                    std::cout << "Handler Called!\n";
                    std::istream stream(&buffer_);
                    std::string received_message;
                    std::getline(stream, received_message, '\0');  // Read until null terminator
                    
                    std::cout << "\nRCV: " << received_message << "\n> ";  // Print received message
                    std::cout.flush();  // Ensure prompt appears properly

                    async_rcv(); // Continue receiving messages
                } else {
                    std::cerr << "Receive error: " << ec.message() << "\n";
                }
            }
        );
    }
    boost::asio::io_context context_;
    boost::asio::ip::tcp::resolver resolver_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::streambuf buffer_; 

};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "\n===== Error usage: ./client SERVER_IP SERVER_PORT ======\n";
        return 1;
    }

    // collect input
    std::string server_ip = argv[1];
    uint16_t server_port = static_cast<uint16_t>(std::stoi(argv[2]));

    try {
        Client client(server_ip, server_port);

        std::thread receive_thread([&client]() { client.start_rcv(); });

        client.start_send();
        receive_thread.join();

    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}