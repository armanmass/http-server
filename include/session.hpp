#ifndef SESSION_HPP
#define SESSION_HPP

#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <iterator>

#include "http_request.hpp"
#include "http_response.hpp"
#include "http_parser.hpp"
#include "request_handler.hpp"

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    //constructor takes ownership of socket
    Session(tcp::socket socket, RequestHandler& handler) : socket_(std::move(socket)), request_handler_(handler){
        std::cout << "Session created." << std::endl;
    }

    ~Session(){ std::cout << "Session destroyed." << std::endl; }

    void start(){ do_read(); }

private:
    void do_read(){
        //create shared ptr so doesn't become deallocated
        //of another thread? lets go of its reference
        auto self(shared_from_this());

        //async read data from buffer
        //lambda is completion handler
        socket_.async_read_some(boost::asio::buffer(buffer_.prepare(1024)),
            [this, self](boost::system::error_code ec, size_t length){
                if(!ec){
                    buffer_.commit(length);
                    auto bufs = buffer_.data();

                    const char* begin_ptr = boost::asio::buffer_cast<const char*>(bufs);
                    size_t buf_size = boost::asio::buffer_size(bufs);
                    const char* end_ptr = begin_ptr + buf_size;

                    auto [result, parsed_end_it] = parser_.parse(request_, begin_ptr, end_ptr);

                    size_t parsed_bytes = std::distance(begin_ptr, parsed_end_it);
                    if(result == HttpRequestParser::ResultType::GOOD){
                        buffer_.consume(parsed_bytes);

                        request_handler_.handle_request(request_, response_);
                        do_write();
                    }else if(result == HttpRequestParser::ResultType::INDETERMINATE){
                        // Need more data, continue reading
                        buffer_.consume(parsed_bytes);
                        do_read();
                    }else{
                        buffer_.consume(buffer_.size());
                        response_ = HttpResponse::stock_response(HttpResponse::bad_request);
                        do_write();
                    }
                }else{
                    if (ec == boost::asio::error::eof){
                        if(parser_.get_state() != HttpRequestParser::State::METHOD_START){
                            response_ = HttpResponse::stock_response(HttpResponse::bad_request);
                            do_write();
                        }else{
                            std::cout << "Client closed connection gracefully. Session ending." << std::endl;
                        }
                    }else{
                        std::cerr << "Read error: " << ec.message() << std::endl;
                    }
                }
            });
    }

    void do_write() {
        auto self(shared_from_this());

        // Convert response to buffers and write
        auto buffers = response_.to_buffers();
        boost::asio::async_write(socket_, buffers,
            [this, self](boost::system::error_code ec, size_t length){
                if(!ec){
                    request_ = {};
                    response_ = {};
                    parser_.reset();

                    do_read();
                }else{
                    std::cerr << "Write error: " << ec.message() << std::endl;
                }
            });
    }

    tcp::socket socket_;
    RequestHandler& request_handler_;

    boost::asio::streambuf buffer_;

    HttpRequestParser parser_;
    HttpRequest request_;
    HttpResponse response_;
};

#endif

