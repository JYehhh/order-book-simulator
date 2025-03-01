#include "Server.hpp"

template <typename ClientSession>
Server<ClientSession>::Server(const std::string& ip, uint16_t port, int thread_count)
    : thread_count_(thread_count), acceptor_(io_context_), adapter_() {

    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address(ip), port);

    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();

    std::cout << "Server is listening on " << ip << ":" << port << "...\n";
}

template <typename ClientSession>
void Server<ClientSession>::start_server() {
    auto next_uid = uid_generator();
    auto session = std::make_shared<ClientSession>(next_uid, io_context_, adapter_);
    adapter.establish_user(next_uid, session);

    acceptor_.async_accept(session->socket(), [=](auto ec) {
        handle_new_connection(session, ec);
    });

    io_context_.run();
}

template <typename ClientSession>
void Server<ClientSession>::handle_new_connection(shared_session_t session, const boost::system::error_code& ec) {
    if (ec) return;

    session->start();

    auto next_session = std::make_shared<ClientSession>(io_context_, adapter_);

    acceptor_.async_accept(next_session->socket(), [=](auto ec) {
        handle_new_connection(next_session, ec);
    });
}

template <typename ClientSession>
UserId Server<ClientSession>::uid_generator() {
    static UserId uid = 0;
    return uid++;
}

// Explicit template instantiation to avoid linker errors
template class Server<class UserSession>;