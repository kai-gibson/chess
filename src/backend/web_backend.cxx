#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

#include <iostream>
#include <string>

#include "../move_generation.h"

namespace beast = boost::beast;      // from <boost/beast.hpp>
namespace http = beast::http;        // from <boost/beast/http.hpp>
namespace net = boost::asio;         // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;    // from <boost/asio/ip/tcp.hpp>

class Chess {
private:
    Arrays a;
    bool white_turn;
    bool in_check;
public:
    Moves m;
    Chess() { 
        m = gen_moves(a.initial_positions); 
        white_turn = true; 
    }

    bool move(u8 start, u8 end) {
        in_check = king_in_danger(m, white_turn);
        
        Move_Piece mp = move_piece(m, start, end, white_turn);
        
        if (!mp.res) return false;

        m = mp.moves;

        white_turn = !white_turn; // successful move, other teams turn

        return false;
    }
};

class http_connection : public std::enable_shared_from_this<http_connection> {
public:
    http_connection(tcp::socket socket, std::unique_ptr<Chess> chess_ptr) 
        : socket_(std::move(socket)),
          m_chess(std::move(chess_ptr)) {}

    // Initiate the asynchronous operations associated with the connection.
    void start() {
        read_request();
        check_deadline();
    }

private:
    // chess board
    std::unique_ptr<Chess> m_chess;

    // The socket for the currently connected client.
    tcp::socket socket_;

    // The buffer for performing reads.
    beast::flat_buffer buffer_{8192};

    // The request message.
    http::request<http::dynamic_body> request_;

    // The response message.
    http::response<http::dynamic_body> response_;

    // The timer for putting a deadline on connection processing.
    net::steady_timer deadline_{socket_.get_executor(), 
                                std::chrono::seconds(60)};

    // Asynchronously receive a complete request message.
    void read_request() {
        auto self = shared_from_this();

        http::async_read(
                socket_, buffer_, request_,
                [self](beast::error_code ec, std::size_t bytes_transferred) {
                    boost::ignore_unused(bytes_transferred);
                    if (!ec) self->process_request();
                });
    }

    // Determine what needs to be done with the request message.
    void process_request() {
        response_.version(request_.version());
        response_.keep_alive(false);

        switch (request_.method()) {
            case http::verb::get:
                response_.result(http::status::ok);
                response_.set(http::field::server, "Beast");
                create_response();
                break;

            default:
                // We return responses indicating an error if
                // we do not recognize the request method.
                response_.result(http::status::bad_request);
                response_.set(http::field::content_type, "text/plain");
                beast::ostream(response_.body())
                        << "Invalid request-method '"
                        << std::string(request_.method_string()) << "'";
                break;
        }

        write_response();
    }

    // Construct a response message based on the program state.
    void create_response() {
        if (request_.target() == "/pieces") {
            response_.set(http::field::content_type, "text/json");
            beast::ostream(response_.body())
                << "\"board\": {";
            
            for (int i=0; i<64; i++) {
                get_bin_num(i);
                std::cout << map_piece(m_chess->m.board.pieces, get_bin_num(i))
                          << "\n";
            }


        } else if (request_.target() == "/time") {
            response_.set(http::field::content_type, "text/json");
            beast::ostream(response_.body())
                    << "<html>\n"
                    << "<head><title>Current time</title></head>\n"
                    << "<body>\n"
                    << "<h1>Current time</h1>\n"
                    << " seconds since the epoch.</p>\n"
                    << "</body>\n"
                    << "</html>\n";
        } else {
            response_.result(http::status::not_found);
            response_.set(http::field::content_type, "text/plain");
            beast::ostream(response_.body()) << "File not found\r\n";
        }
    }

    // Asynchronously transmit the response message.
    void write_response() {
        auto self = shared_from_this();
        response_.content_length(response_.body().size());

        http::async_write(
                socket_, response_, [self](beast::error_code ec, std::size_t) {
                    self->socket_.shutdown(tcp::socket::shutdown_send, ec);
                    self->deadline_.cancel();
                });
    }

    // Check whether we have spent enough time on this connection.
    void check_deadline() {
        auto self = shared_from_this();

        deadline_.async_wait([self](beast::error_code ec) {
            if (!ec) {
                // Close socket to cancel any outstanding operation.
                self->socket_.close(ec);
            }
        });
    }
};

// "Loop" forever accepting new connections.
void http_server(tcp::acceptor& acceptor, tcp::socket& socket, Chess& ch) {
    acceptor.async_accept(socket, 
            [&](beast::error_code ec) {
                if (!ec) { 
                    std::make_shared<http_connection>(std::move(socket), 
                        std::make_unique<Chess>(ch))->start();
                }
                http_server(acceptor, socket, ch);
            });
}

int main(int argc, char** argv) {
    Chess chess;

    try {
        auto const address = net::ip::make_address("127.0.0.1");
        unsigned short port = 8100;

        net::io_context ioc{1};

        tcp::acceptor acceptor{ioc, {address, port}};
        tcp::socket socket{ioc};
        http_server(acceptor, socket, chess);

        ioc.run();

    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
