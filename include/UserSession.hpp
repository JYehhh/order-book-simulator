#pragma once

#include <iostream>
#include <deque>
#include <memory>
#include <boost/asio.hpp>
#include "Adapter.hpp"
#include "Types.hpp"
#include "IObserver.hpp" // Include the header file for Listener

class UserSession : public std::enable_shared_from_this<UserSession>, public IObserver {
public:
    explicit UserSession(UserId uid, boost::asio::io_context& ctx, Adapter& adapter);

    boost::asio::ip::tcp::socket& socket();
    void start();
    void send_packet(const std::string& msg);
    void update(const std::string &message) override;

private:
    // Packet Sending Helpers
    void queue_message(std::string message);
    void start_packet_send();
    void handle_send_packet(const boost::system::error_code& ec);

    // Packet Reading Helpers
    void read_packet();
    void handle_read_packet(const boost::system::error_code& ec, std::size_t n_bytes);

    UserId uid_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::io_context::strand write_strand_;
    boost::asio::streambuf buffer_;
    Adapter& adapter_;

    std::deque<std::string> send_packet_queue;
};