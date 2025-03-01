#pragma once

#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>
#include "OrderJsonEncoder.hpp"

// #include <cereal/archives/json.hpp>
// #include <cereal/types/vector.hpp>


#define USER_PROMPT "> "
#define INSTRUCTIONS \
    "Enter an order command in the following format:\n" \
    "  - Add Order: ADD BUY/SELL PRICE QUANTITY\n" \
    "  - Cancel Order: CANCEL ORDER_ID\n" \
    "  - Modify Order: MODIFY ORDER_ID NEW_PRICE BUY/SELL NEW_QUANTITY\n" \
    "  - Display Order Book: DISPLAY\n" \
    "  - Display User Orders: ORDERS\n" \
    "Type 'exit' to quit.\n"

class Client : public std::enable_shared_from_this<Client> {
public:
    Client(const std::string& ip, uint16_t port);
    void start();

private:
    void async_rcv();
    void async_prompt_user();
    void async_send(std::string message);

    boost::asio::io_context context_;
    boost::asio::ip::tcp::resolver resolver_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::streambuf buffer_;  // Buffer for incoming messages

    boost::asio::posix::stream_descriptor input_stream_;  // Handles stdin asynchronously
    boost::asio::streambuf input_buffer_;  // Buffer for stdin input
};
