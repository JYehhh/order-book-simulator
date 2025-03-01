#include "UserSession.hpp"

UserSession::UserSession(UserId uid, boost::asio::io_context& ctx, IParamsProcessor& processor)
    : uid_(uid)
    , socket_(ctx)
    , write_strand_(ctx)
    , processor_(processor) {}

boost::asio::ip::tcp::socket& UserSession::socket() {
    return socket_;
}

UserId UserSession::get_user_id() const {
    return uid_;
}

void UserSession::start() {
    std::cout << "User Connected!\n";
    read_packet();
}

void UserSession::send_packet(const std::string& msg) {
    boost::asio::post(write_strand_, [me = shared_from_this(), msg]() {
        me->queue_message(msg);
    });
}

// ---- Packet Sending Helpers ----
void UserSession::queue_message(std::string message) {
    bool write_in_progress = !send_packet_queue.empty();
    send_packet_queue.push_back(std::move(message));
    if (!write_in_progress) {
        start_packet_send();
    }
}

void UserSession::start_packet_send() {
    send_packet_queue.front() += '\0'; // Append null terminator
    boost::asio::async_write(
        socket_,
        boost::asio::buffer(send_packet_queue.front()),
        boost::asio::bind_executor(write_strand_,
            [me = shared_from_this()](boost::system::error_code ec, std::size_t) {
                me->handle_send_packet(ec);
            }
        )
    );
}

void UserSession::handle_send_packet(const boost::system::error_code& ec) {
    if (!ec) {
        send_packet_queue.pop_front();
        if (!send_packet_queue.empty()) {
            start_packet_send();
        }
    } else {
        std::cerr << "Send error: " << ec.message() << std::endl;
    }
}

// ---- Packet Reading Helpers ----
void UserSession::read_packet() {
    boost::asio::async_read_until(
        socket_,
        buffer_,
        '\0',
        [me = shared_from_this()](boost::system::error_code ec, std::size_t n_bytes) {
            me->handle_read_packet(ec, n_bytes);
        }
    );
}

void UserSession::handle_read_packet(const boost::system::error_code& ec, std::size_t n_bytes) {
    if (ec) return;

    std::istream stream(&buffer_);
    std::string json_message;
    std::getline(stream, json_message, '\0');

    try {
        Request req(uid_, json_message);
        Response res = processor_.process_request(req);

        if (res.ec == ErrorCode::ERROR) {
            send_packet("Error from Processor: ");
        }
        
        send_packet(res.print_string);
    } catch (const std::exception& e) {
        std::cerr << "CRITICAL-UNCAUGHT-ERROR (paramsprocessor.process_params): " << e.what() << std::endl;
        send_packet("CRITICAL-UNCAUGHT-ERROR (paramsprocessor.process_params)");
    }

    read_packet();
}

void UserSession::update(const std::string &message) {
    send_packet(message);
}