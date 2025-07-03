#ifndef REQUEST_HANDLER_HPP
#define REQUEST_HANDLER_HPP

#include <string>
#include "http_request.hpp"
#include "http_response.hpp"

class RequestHandler{
public:
    explicit RequestHandler(const std::string& doc_root);
    void handle_request(const HttpRequest& req, HttpResponse& res);

private:
    std::string doc_root_;
    static bool url_decode(const std::string& in, std::string& out);
};

#endif