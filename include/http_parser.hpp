#ifndef HTTP_PARSER_HPP
#define HTTP_PARSER_HPP

#include "http_request.hpp"
#include <tuple>

class HttpRequestParser{
public:
    enum class ResultType {GOOD, BAD, INDETERMINATE};

    template <typename InputIterator>
    std::tuple<ResultType, InputIterator> parse(
        HttpRequest& req,
        InputIterator begin,
        InputIterator end
    ) {
        return {ResultType::BAD, begin};
    }
};

#endif