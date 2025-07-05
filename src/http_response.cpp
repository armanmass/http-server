#include "http_response.hpp"
#include <string>
#include <vector>

namespace status_strings {
    const std::string ok = "HTTP/1.1 200 OK\r\n";
    const std::string not_found = "HTTP/1.1 404 Not Found\r\n";
    const std::string bad_request = "HTTP/1.1 400 Bad Request\r\n";
    
    boost::asio::const_buffer to_buffer(HttpResponse::StatusType status){
        switch(status){
            case HttpResponse::ok: return boost::asio::buffer(ok);
            case HttpResponse::not_found: return boost::asio::buffer(not_found);
            default: return boost::asio::buffer(bad_request);
        }
    }
}

namespace stock_replies {
    const char ok[] = "";
    const char not_found[] =
        "<html>"
        "<head><title>Not Found</title></head>"
        "<body><h1>404 Not Found</h1></body>"
        "</html>";
    const char bad_request[] =
        "<html>"
        "<head><title>Bad Request</title></head>"
        "<body><h1>400 Bad Request</h1></body>"
        "</html>";
    
    std::string to_string(HttpResponse::StatusType status){
        switch(status){
            case HttpResponse::ok: return ok;
            case HttpResponse::not_found: return not_found;
            default: return bad_request;
        }
    }
}

std::vector<boost::asio::const_buffer> HttpResponse::to_buffers(){
    std::vector<boost::asio::const_buffer> buffers;
    buffers.push_back(status_strings::to_buffer(status));

    for(const auto& h : headers){
        buffers.push_back(boost::asio::buffer(h.first));
        buffers.push_back(boost::asio::buffer(": "));
        buffers.push_back(boost::asio::buffer(h.second));
        buffers.push_back(boost::asio::buffer("\r\n"));
    }

    buffers.push_back(boost::asio::buffer("\r\n"));
    buffers.push_back(boost::asio::buffer(content));
    return buffers;
}

HttpResponse HttpResponse::stock_response(HttpResponse::StatusType status){
    HttpResponse res;
    res.status = status;
    res.content = stock_replies::to_string(status);

    res.headers.resize(3);
    res.headers[0].first = "Content-Length";
    res.headers[0].second = std::to_string(res.content.size());
    res.headers[1].first = "Content-Type";
    res.headers[1].second = "text/html";
    res.headers[2].first = "Connection";
    res.headers[2].second = "close";
    return res;
}