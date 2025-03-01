#pragma once

#include <iostream>
#include <deque>
#include <vector>
#include <memory>
#include <thread>
#include <boost/asio.hpp>
#include "Adapter.hpp"

#define DEFAULT_PORT 5000

template <typename ClientSession>
class Server {
public:
    Server(const std::string& ip, uint16_t port, int thread_count = 1);
    void start_server();

private:
    using shared_session_t = std::shared_ptr<ClientSession>;
    
    void handle_new_connection(shared_session_t session, const boost::system::error_code& ec);

    UserId uid_generator();

    int thread_count_;
    std::vector<std::thread> thread_pool_;
    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;
    Adapter adapter_;
};