#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

void handle_connection(tcp::socket socket){
    try {
        while(true){
            // buffer
            char data[1024];

            //wait for data
            size_t length = socket.read_some(boost::asio::buffer(data));

            //write
            boost::asio::write(socket, boost::asio::buffer(data, length));
        }
    } catch (const boost::system::system_error& e) {
        if(e.code() == boost::asio::error::eof){
            std::cout << "Client disconnected without error." << std::endl;
        }else{
            std::cerr << "Error in connection: " << e.what() << std::endl;
        }
    }
}

int main(){
    try {
        boost::asio::io_context io_context;

        //listen for new incoming connections
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));
        std::cout << "Server listening on port 8080..." << std::endl;

        while(true){
            //block until new connection made
            tcp::socket socket(io_context);
            acceptor.accept(socket);

            std::cout << "Accepted new connection." << std::endl;

            // handle connection (one at a time)
            handle_connection(std::move(socket));
        }
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
    }

    return 0;
}