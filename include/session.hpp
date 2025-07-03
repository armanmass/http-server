#ifndef SESSION_HPP
#define SESSION_HPP

#include <boost/asio.hpp>
#include <iostream>
#include <memory>

using boost::asio::ip::tcp:

class Session : public std::enable_shared_from_this<Session> {
public:
    //constructor takes ownershiper of socket
    Session(tcp::socket socket) : socket_(std::move(socket)) {}

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
                    do_write(length);
                }else{
                    std::cerr << "Read error: " << ec.message() << std::endl;
                }
            });
    }

    void do_write(size_t length) {
        auto self(shared_from_this());

        boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
            [this, self](boost::system::error_code ec, size_t length){
                if(!ec){
                    do_read()
                }else{
                    std::cerr << "Write error: " << ec.message() << std::endl;
                }
            });
    }

    tcp:: socket socket_;
    static constexpr size_t max_length = 1024;
    char data[max_length];
};

#endif

