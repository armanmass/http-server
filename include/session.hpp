#ifndef SESSION_HPP
#define SESSION_HPP

#include <boost/asio.hpp>
#include <iostream>
#include <memory>

#include "http_request.hpp"
#include "http_response.hpp"
#include "http_parser.hpp"
#include "request_handler.hpp"

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    //constructor takes ownershiper of socket
    Session(tcp::socket socket, RequestHandler& handler) : socket_(std::move(socket)), request_handler_(handler){
        std::cout << "Session created." << std::endl;
    }

    ~Session(){ std::cout << "Session destroyed." << std::endl; }

    void start(){ do_read(); }

private:
    void do_read(){
        //create shared ptr so doesn't become deallocated
        //if another thread? lets go of its reference
        auto self(shared_from_this());

        //async read data from buffer
        //lambda is completion handler
        socket_.async_read_some(boost::asio::buffer(buffer_.prepare(1024)),
            [this, self](boost::system::error_code ec, size_t length){
                if(!ec){
                    buffer_.commit(length);
                    std::istream request_stream(&buffer_);
                    std::string request_string((std::istreambuf_iterator<char>(request_stream)), std::istreambuf_iterator<char>());

                    auto [result, _] = parser_.parse(request_, request_string.begin(), request_string.end());

                    if(result == HttpRequestParser::ResultType::GOOD){
                        request_handler_.handle_request(request_, response_);
                        do_write();
                    }else{
                        response_ = HttpResponse::stock_response(HttpResponse::bad_request);
                        do_write();
                    }
                }else if (ec != boost::asio::error::eof){

                    std::cerr << "Read error: " << ec.message() << std::endl;
                }
            });
    }

    void do_write() {
        auto self(shared_from_this());

        boost::asio::async_write(socket_, buffer_,
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

