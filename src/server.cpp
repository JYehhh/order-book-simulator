#include "Server.hpp"

template <typename ClientSession>
Server<ClientSession>::Server(const std::string& ip, uint16_t port, int thread_count)
    : thread_count_(thread_count)
    , acceptor_(io_context_)
    , processor_()
    {

    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address(ip), port);

    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();

    std::cout << "Server is listening on " << ip << ":" << port << "...\n";
}

template <typename ClientSession>
void Server<ClientSession>::start_accept() {
    auto next_uid = uid_generator();
    auto session = std::make_shared<ClientSession>(next_uid, io_context_, processor_);

    acceptor_.async_accept(session->socket(), [this, session](auto ec) {
        handle_new_connection(session, ec);
    });
}

template <typename ClientSession>
void Server<ClientSession>::start_server() {
    start_accept();

    io_context_.run();
}

template <typename ClientSession>
void Server<ClientSession>::handle_new_connection(shared_session_t session, const boost::system::error_code& ec) {
    if (ec) {
        std::cerr << "Error accepting connection: " << ec.message() << "\n";
        return;
    }

    processor_.register_user(session->get_user_id(), session.get());
    session->start();

    start_accept();
}

template <typename ClientSession>
UserId Server<ClientSession>::uid_generator() {
    static UserId uid = 0;
    return uid++;
}

// Explicit template instantiation to avoid linker errors
template class Server<class UserSession>;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "\n===== Error usage: ./server SERVER_IP SERVER_PORT ======\n";
        return 1;
    }

    std::string ip = argv[1];
    uint16_t port = static_cast<uint16_t>(std::stoi(argv[2]));

    try {
        Server<UserSession> server(ip, port);
        server.start_server();
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}