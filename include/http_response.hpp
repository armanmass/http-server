#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <string>
#include <vector>
#include <map>
#include <boost/asio.hpp>

struct HttpResponse{
    enum StatusType {
        ok = 200,
        bad_request = 400,
        not_found = 404
    } status;

    std::vector<std::pair<std::string, std::string>> headers;
    std::string content;

    std::vector<boost::asio::const_buffer> to_buffers();
    static HttpResponse stock_response(StatusType status);
};

#endif