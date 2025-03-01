#include "Client.hpp"

Client::Client(const std::string& ip, uint16_t port)
    : resolver_(context_)
    , socket_(context_)
    , input_stream_(context_, ::dup(STDIN_FILENO)) 
{
    auto endpoints = resolver_.resolve(ip, std::to_string(port));
    boost::asio::connect(socket_, endpoints);
    std::cout << "Connected to server at " << ip << ":" << port << "\n";
    std::cout << "Type 'instructions' for usage instructions!\n";
}

void Client::start() {
    async_rcv();         // Start receiving messages
    async_prompt_user(); // Start reading user input
    context_.run();      // Run the event loop
}

void Client::async_rcv() {
    boost::asio::async_read_until(
        socket_, buffer_, '\0',
        [self = shared_from_this()](boost::system::error_code ec, [[maybe_unused]] std::size_t bytes_transferred) {
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

void Client::async_prompt_user() {
    std::cout << USER_PROMPT;
    std::cout.flush();

    boost::asio::async_read_until(
        input_stream_, input_buffer_, '\n',
        [self = shared_from_this()](boost::system::error_code ec, [[maybe_unused]] std::size_t bytes_transferred) {
            if (!ec) {
                std::istream stream(&self->input_buffer_);
                std::string message;
                std::getline(stream, message);  // Read user input

                if (message == "instructions") {
                    std::cout << INSTRUCTIONS;
                    self->async_prompt_user();
                    return;
                }

                if (message == "exit") {
                    self->socket_.close();
                    return;
                }
                
                try {
                    std::string json_message = OrderJsonEncoder::make_order(message);
                    self->async_send(std::move(json_message));
                } catch (const std::exception &e) {
                    std::cerr << "Client-Side Error: " << e.what() << "\n";
                    self->async_prompt_user();
                }

            } else {
                std::cerr << "Input read error: " << ec.message() << "\n";
            }
        }
    );
}

void Client::async_send(std::string message) {
    message += '\0';  // Ensure null termination
    boost::asio::async_write(
        socket_, boost::asio::buffer(message),
        [self = shared_from_this()](boost::system::error_code ec, [[maybe_unused]] std::size_t bytes_transferred) {
            if (!ec) {
                self->async_prompt_user();  // Reprompt the user for input
            } else {
                std::cerr << "Send-Error: " << ec.message() << "\n";
            }
        }
    );
}

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