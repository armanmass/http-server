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
        socket_.async_read_some(boost::asio::buffer(data, max_length),
            [this, self](boost::system::error_code ec, size_t length){
                if(!ec){
                    auto [result,_] = parser_.parse(request_,data,data+length);

                    if(result == HttpRequestParser::ResultType::GOOD){
                        request_handler_.handle_request(request_, response_);
                        do_write(length);
                    }else if(result == HttpRequestParser::ResultType::BAD){
                        response_ = HttpResponse::stock_response(HttpResponse::bad_request);
                        do_write(length);
                    }else{
                        do_read();
                    }
                }else if (ec != boost::asio::error::eof){

                    std::cerr << "Read error: " << ec.message() << std::endl;
                }
            });
    }

    void do_write(size_t length) {
        auto self(shared_from_this());

        boost::asio::async_write(socket_, boost::asio::buffer(data, length),
            [this, self](boost::system::error_code ec, size_t length){
                if(!ec){
                    boost::system::error_code ignored_ec;
                    socket_.shutdown(tcp::socket::shutdown_both, ignored_ec);
                }else{
                    std::cerr << "Write error: " << ec.message() << std::endl;
                }
            });
    }

    tcp:: socket socket_;
    static constexpr size_t max_length = 1024;
    char data[max_length];

    RequestHandler& request_handler_;
    HttpRequestParser parser_;
    HttpRequest request_;
    HttpResponse response_;
};

#endif

