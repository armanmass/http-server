#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio.hpp>
#include "session.hpp"

using boost::asio::ip::tcp;

class Server{
public:
    Server(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
            do_accept();
        }
private:
    void do_accept(){
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket){
                if(!ec){
                    std::cout << "Accepted connection. Creating session." << std::endl;
                    std::make_shared<Session>(std::move(socket))->start();
                }else{
                    std::cerr << "Accept error: " << ec.message() << std::endl;
                }

                do_accept();
            });
    }

    tcp::acceptor acceptor_;
};

#endif