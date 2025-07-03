#include <iostream>
#include <vector>
#include <thread>

#include <boost/asio.hpp>
#include "server.hpp"

int main(int argc, char* argv[]){
    try {
        if(argc != 2){
            std::cerr << "Usage: http_server <doc_root>\n";
            return -1;
        }
        const std::string doc_root = argv[1];
        const short port = 8080;
        boost::asio::io_context io_context;

        Server s(io_context, port, doc_root);
        std::cout << "Server listening on port " << port << "." << std::endl;

        const short num_threads = 4;
        std::vector<std::thread> threads;

        for(int i=0; i<num_threads; i++){
            threads.emplace_back([&io_context](){
                std::cout << "Thread " << std::this_thread::get_id() << " started." << std::endl;
                io_context.run();
                std::cout << "Thread " << std::this_thread::get_id() << " finished." << std::endl;
            });
        }

        io_context.run();

        for(auto& t : threads) t.join();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}