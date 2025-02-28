#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>
#include "Enums.hpp"

// #include <cereal/archives/json.hpp>
// #include <cereal/types/vector.hpp>

#define USER_PROMPT "> "
#define INSTRUCTIONS "Enter an order command in the following format:\n  - Add Order: ADD BUY/SELL PRICE QUANTITY\n  - Cancel Order: CANCEL ORDER_ID\n  - Modify Order: MODIFY ORDER_ID NEW_PRICE BUY/SELL NEW_QUANTITY\nType 'exit' to quit.\n"

class OrderJsonGenerator {
    public:
    static std::string make_order(std::vector<std::string> &params) {
        
        if (params.empty()) {
            std::cerr << "Invalid command. Please enter a valid order.\n";
            throw std::invalid_argument("Invalid command. Please enter a valid order.\n");
            return "";
        }

        std::string order_type = params[0];
        std::string json_message;

        if (order_type == "ADD") {
            json_message = make_add_order(params);
        } else if (order_type == "CANCEL") {
            json_message = make_cancel_order(params);
        } else if (order_type == "MODIFY") {
            json_message = make_modify_order(params);
        } else {
            throw std::runtime_error("Unknown command. Valid commands: ADD, CANCEL, MODIFY.");
        }

        return json_message;
    }

    private:
    static std::string make_add_order(std::vector<std::string> &params) {
        if (params.size() != 4) {
            throw std::runtime_error("Invalid ADD order format. Expected: ADD BUY/SELL PRICE QUANTITY");
        }

        std::string command = params[0];
        std::string side = params[1];  // BUY or SELL
        Price price = std::stod(params[2]);
        Quantity quantity = std::stoi(params[3]);

        if (price <= 0 || quantity <= 0) {
            throw std::runtime_error("Price and Quantity must be positive numbers.");
        }

        nlohmann::json add_order = {
            {"command", command}, 
            {"side", side}, 
            {"price", price}, 
            {"quantity", quantity},
        };
        return add_order.dump();
    };

    static std::string make_cancel_order(std::vector<std::string> &params) {
        if (params.size() != 2) {
            throw std::runtime_error("Invalid CANCEL order format. Expected: CANCEL ORDER_ID");
        }

        std::string command = params[0];
        OrderId order_id = std::stoi(params[1]);

        if (order_id < 0) {
            throw std::runtime_error("Order ID must be a positive number.");
        }

        nlohmann::json cancel_order = {
            {"command", command}, 
            {"order_id", order_id},
        };
        return cancel_order.dump();
    }

    static std::string make_modify_order(std::vector<std::string> &params) {
        std::ostringstream json_stream;
        if (params.size() != 5) {
            throw std::runtime_error("Invalid MODIFY order format. Expected: MODIFY ORDER_ID NEW_PRICE BUY/SELL NEW_QUANTITY");
        }

        std::string command = params[0];
        int order_id = std::stoi(params[1]);
        Price new_price = std::stod(params[2]);
        std::string new_side = params[3];  // BUY or SELL
        OrderId new_quantity = std::stoi(params[4]);

        if (order_id < 0 || new_price <= 0 || new_quantity <= 0) {
            throw std::runtime_error("Order ID must be positive, and Price/Quantity must be greater than zero.");
        }

        // Serialize to JSON
        nlohmann::json modify_order = {
            {"command", command}, 
            {"order_id", order_id},
            {"new_side", new_side}, 
            {"new_price", new_price}, 
            {"new_quantity", new_quantity},
        };
        return modify_order.dump();
    }

};

class Client : public std::enable_shared_from_this<Client> {
public:
    Client(const std::string& ip, uint16_t port) 
        : resolver_(context_), socket_(context_), input_stream_(context_, ::dup(STDIN_FILENO)) {  // Attach stdin to async IO
        auto endpoints = resolver_.resolve(ip, std::to_string(port));
        boost::asio::connect(socket_, endpoints);
        std::cout << "Connected to server at " << ip << ":" << port << "\n";
        std::cout << "Type 'instructions' for usage instructions!\n";
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

                    if (message == "instructions") {
                        std::cout << INSTRUCTIONS;
                        self->async_prompt_user();
                        return;
                    }

                    if (message == "exit") {
                        self->socket_.close();
                        return;
                    }

                    std::istringstream iss(message);
                    std::vector<std::string> params;
                    std::string p;

                    while (iss >> p) params.push_back(p);
                    
                    try {
                        std::string json_message = OrderJsonGenerator::make_order(params);
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

    void async_send(std::string message) {
        message += '\0';  // Ensure null termination
        boost::asio::async_write(
            socket_, boost::asio::buffer(message),
            [self = shared_from_this()](boost::system::error_code ec, std::size_t bytes_transferred) {
                if (!ec) {
                    self->async_prompt_user();  // Reprompt the user for input
                } else {
                    std::cerr << "Send-Error: " << ec.message() << "\n";
                }
            }
        );
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