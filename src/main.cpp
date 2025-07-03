#include <iostream>
#include <vector>
#include <thread>

#include <boost/asio.hpp>
#include "server.hpp"

int main(){
    try {
        const short port = 8080;
        boost::asio::io_context io_context;

        Server s(io_context, port);
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