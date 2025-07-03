#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <vector>
#include <map>

struct HttpRequest {
    std::map<std::string, std::string> headers;

    int http_version_major;
    int http_version_minor;

    std::string method;
    std::string body;
    std::string uri;
};

#endif