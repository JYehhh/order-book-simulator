#include <iostream>
#include <boost/asio.hpp>

#define USER_PROMPT "> "

class Client : public std::enable_shared_from_this<Client> {
public:
    Client(const std::string& ip, uint16_t port) 
        : resolver_(context_), socket_(context_), input_stream_(context_, ::dup(STDIN_FILENO)) {  // Attach stdin to async IO
        auto endpoints = resolver_.resolve(ip, std::to_string(port));
        boost::asio::connect(socket_, endpoints);
        std::cout << "Connected to server at " << ip << ":" << port << "\n";
    }

    void start() {
        async_rcv();         // Start receiving messages
        async_prompt_user(); // Start reading user input
        context_.run();      // Run the event loop
    }

private:
    void async_rcv() {
        boost::asio::async_read_until(
            socket_, buffer_, '\0',
            [self = shared_from_this()](boost::system::error_code ec, std::size_t bytes_transferred) {
                if (!ec) {
                    std::istream stream(&self->buffer_);
                    std::string received_message;
                    std::getline(stream, received_message, '\0');  // Read until null terminator
                    
                    std::cout << "\nRCV: " << received_message << "\n";  // Print received message
                    std::cout << USER_PROMPT;
                    std::cout.flush();  // Ensure prompt appears properly

                    self->async_rcv();  // Continue receiving messages
                } else {
                    std::cerr << "Receive error: " << ec.message() << "\n";
                }
            }
        );
    }

    void async_prompt_user() {
        std::cout << USER_PROMPT;
        std::cout.flush();

        boost::asio::async_read_until(
            input_stream_, input_buffer_, '\n',
            [self = shared_from_this()](boost::system::error_code ec, std::size_t bytes_transferred) {
                if (!ec) {
                    std::istream stream(&self->input_buffer_);
                    std::string message;
                    std::getline(stream, message);  // Read user input

                    if (message == "exit") {
                        self->socket_.close();
                        return;
                    }

                    self->async_send(std::move(message));
                } else {
                    std::cerr << "Input read error: " << ec.message() << "\n";
                }
            }
        );
    }

    void async_send(std::string message) {
        message += '\0';  // Ensure null termination
        boost::asio::async_write(
            socket_, boost::asio::buffer(message),
            [self = shared_from_this()](boost::system::error_code ec, std::size_t bytes_transferred) {
                if (!ec) {
                    self->async_prompt_user();  // Reprompt the user for input
                } else {
                    std::cerr << "Send error: " << ec.message() << "\n";
                }
            }
        );
        // std::istream stream(&buffer_);
        // std::string received_message(std::istreambuf_iterator<char>(stream), {});
        // std::cout << "Raw received: " << received_message << std::endl;
    }

    boost::asio::io_context context_;
    boost::asio::ip::tcp::resolver resolver_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::streambuf buffer_;  // Buffer for incoming messages

    boost::asio::posix::stream_descriptor input_stream_;  // Handles stdin asynchronously
    boost::asio::streambuf input_buffer_;  // Buffer for stdin input
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "\n===== Error usage: ./client SERVER_IP SERVER_PORT ======\n";
        return 1;
    }

    std::string server_ip = argv[1];
    uint16_t server_port = static_cast<uint16_t>(std::stoi(argv[2]));

    try {
        auto client = std::make_shared<Client>(server_ip, server_port);
        client->start();
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}