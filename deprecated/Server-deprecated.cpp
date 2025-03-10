#include <iostream>
#include <deque>
#include "OrderBook.hpp"
#include "Enums.hpp"
#include "Adapter.hpp"

#include <boost/asio.hpp>

#define STD_BUF_SIZE 1024
#define DEFAULT_PORT 5000

template <typename ClientSession> 
class Server {
    using shared_session_t = std::shared_ptr<ClientSession>;

    public:
    Server(const std::string& ip, uint16_t port, int thread_count=1)
        : thread_count_(thread_count), acceptor_(io_context_), adapter_() {
        
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address(ip), port);
        
        acceptor_.open(endpoint.protocol()); // Open acceptor with protocol (TCP)
        acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true)); // Allow address reuse
        acceptor_.bind(endpoint);
        acceptor_.listen();

        std::cout << "Server is listening on " << ip << ":" << port << "...\n";
    }

    void start_server() {
        auto session = std::make_shared<ClientSession>(io_context_, ob_);

        acceptor_.async_accept(session->socket(), [=](auto ec) {
            handle_new_connection(session, ec);
        });

        io_context_.run();
    }

    private:
    void handle_new_connection(shared_session_t session, boost::system::error_code const &ec) {
        if (ec) { return; }

        session->start();

        auto next_session = std::make_shared<ClientSession> (io_context_, ob_);

        acceptor_.async_accept(next_session->socket(), [=](auto ec) {
            handle_new_connection(next_session, ec);
        });
    };

    int thread_count_;
    std::vector<std::thread> thread_pool_;
    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;
    Adapter adapter_;
    // OrderBook ob_;
};

class UserSession : public std::enable_shared_from_this<UserSession> {
    public:
    // UserSession(boost::asio::io_context &ctx, OrderBook &ob) 
    //     : socket_(ctx)
    //     , write_strand_(ctx)
    //     , ob_(ob) {};

    UserSession(boost::asio::io_context &ctx, Adapter &adapter) 
        : socket_(ctx)
        , write_strand_(ctx)
        , adapter_(adapter) {};


    boost::asio::ip::tcp::socket& socket() {
        return socket_;
    }

    void start() {
        std::cout << "User Connected!\n";
        read_packet();
    }

    void send_packet(std::string msg) {
        boost::asio::post(write_strand_, [me = shared_from_this(), msg = std::move(msg)]() {
            me->queue_message(msg);
        });
    }

    private:
    // SENDING PACKET HELPERS
    void queue_message(std::string message) {
        // if the queue is empty, hasn't been writing, thus start writing
        bool write_in_progress = !send_packet_queue.empty();
        send_packet_queue.push_back(std::move(message));
        if (!write_in_progress) {
            start_packet_send();
        }
    }

    void start_packet_send() {
        // std::cout << "Sending Packet!\n";
        send_packet_queue.front() += '\0';
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

    void handle_send_packet(boost::system::error_code const &error) {
        if (!error) {
            send_packet_queue.pop_front();
            if(!send_packet_queue.empty()) {
                start_packet_send();
            }
        } else {
            std::cerr << "Send error: " << error.message() << std::endl;
        }
    }

    // READING PACKETS
    void read_packet() {
        boost::asio::async_read_until(
            socket_,
            buffer_,
            '\0',
            [me=shared_from_this()] (boost::system::error_code const &ec, std::size_t n_bytes) {
                me->handle_read_packet(ec, n_bytes);
            }
        );
    }

    void handle_read_packet(boost::system::error_code const &ec, std::size_t n_bytes) {
        if (ec) return;

        std::istream stream(&buffer_);
        std::string message;
        std::getline(stream, message, '\0');

        // std::cout << "RCV (" << n_bytes << " bytes): " << message << std::endl;
        
        try {
            Response res = adapter_.process_string(message);
            
            if (res.ec == ErrorCode::ERROR) {
                send_packet("Error From OrderBook: ");
            }
            
            send_packet(res.print_string);

        } catch (const std::exception &e) {
            std::cerr << "CRITICAL-UNCAUGHT-ERROR (adapter.process_params): " << e.what() << std::endl;
            send_packet("CRITICAL-UNCAUGHT-ERROR (adapter.process_params)");
        }

        read_packet();
    }

    // boost::asio::io_context &context_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::io_context::strand write_strand_; // only one completion handler wrapped by this strand will run at any one time
    boost::asio::streambuf buffer_;
    Adapter &adapter_;
    // OrderBook &ob_;

    std::deque<std::string> send_packet_queue;
};

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