#ifndef HTTP_PARSER_HPP
#define HTTP_PARSER_HPP

#include "http_request.hpp"
#include <tuple>

class HttpRequestParser{
public:
    enum class ResultType {GOOD, BAD, INDETERMINATE};

    void reset() { state_ = METHOD_START; }

    template <typename InputIterator>
    std::tuple<ResultType, InputIterator> parse(
        HttpRequest& req,
        InputIterator begin,
        InputIterator end
    ) {
        while(begin != end){
            ResultType result = consume(req, *begin);
            begin++;
            if(result != ResultType::INDETERMINATE){
                return {result, begin};
            }
        }
        return {ResultType::BAD, begin};
    }

private:
    // ai generated function
    // state machine to process text
    ResultType consume(HttpRequest& req, char input){
        switch (state_) {
        case METHOD_START:
            if (!is_char(input) || is_ctl(input) || is_tspecial(input)) { return ResultType::BAD; }
            state_ = METHOD;
            req.method.push_back(input);
            return ResultType::INDETERMINATE;
        case METHOD:
            if (input == ' ') { state_ = URI; return ResultType::INDETERMINATE; }
            if (!is_char(input) || is_ctl(input) || is_tspecial(input)) { return ResultType::BAD; }
            req.method.push_back(input);
            return ResultType::INDETERMINATE;
        case URI:
            if (input == ' ') { state_ = HTTP_VERSION_H; return ResultType::INDETERMINATE; }
            if (is_ctl(input)) { return ResultType::BAD; }
            req.uri.push_back(input);
            return ResultType::INDETERMINATE;
        case HTTP_VERSION_H:
            if (input == 'H') { state_ = HTTP_VERSION_T1; return ResultType::INDETERMINATE; } return ResultType::BAD;
        case HTTP_VERSION_T1:
            if (input == 'T') { state_ = HTTP_VERSION_T2; return ResultType::INDETERMINATE; } return ResultType::BAD;
        case HTTP_VERSION_T2:
            if (input == 'T') { state_ = HTTP_VERSION_P; return ResultType::INDETERMINATE; } return ResultType::BAD;
        case HTTP_VERSION_P:
            if (input == 'P') { state_ = HTTP_VERSION_SLASH; return ResultType::INDETERMINATE; } return ResultType::BAD;
        case HTTP_VERSION_SLASH:
            if (input == '/') { state_ = HTTP_VERSION_MAJOR; req.http_version_major = 0; return ResultType::INDETERMINATE; } return ResultType::BAD;
        case HTTP_VERSION_MAJOR:
            if (input == '.') { state_ = HTTP_VERSION_MINOR; req.http_version_minor = 0; return ResultType::INDETERMINATE; }
            if (is_digit(input)) { req.http_version_major = req.http_version_major * 10 + (input - '0'); return ResultType::INDETERMINATE; }
            return ResultType::BAD;
        case HTTP_VERSION_MINOR:
            if (input == '\r') { state_ = EXPECTING_NEWLINE_1; return ResultType::INDETERMINATE; }
            if (is_digit(input)) { req.http_version_minor = req.http_version_minor * 10 + (input - '0'); return ResultType::INDETERMINATE; }
            return ResultType::BAD;
        case EXPECTING_NEWLINE_1:
            if (input == '\n') { state_ = HEADER_LINE_START; return ResultType::INDETERMINATE; } return ResultType::BAD;
        case HEADER_LINE_START:
            if (input == '\r') { state_ = EXPECTING_NEWLINE_3; return ResultType::INDETERMINATE; }
            if (!req.headers.empty() && (input == ' ' || input == '\t')) { state_ = HEADER_LWS; return ResultType::INDETERMINATE; }
            if (!is_char(input) || is_ctl(input) || is_tspecial(input)) { return ResultType::BAD; }
            req.headers.emplace_back("", "");
            req.headers.back().first.push_back(input);
            state_ = HEADER_NAME;
            return ResultType::INDETERMINATE;
        case HEADER_LWS: // Obsolete line folding
            if (input == '\r') { state_ = EXPECTING_NEWLINE_2; return ResultType::INDETERMINATE; }
            if (input == ' ' || input == '\t') return ResultType::INDETERMINATE;
            if (is_ctl(input)) return ResultType::BAD;
            state_ = HEADER_VALUE;
            req.headers.back().second.push_back(input);
            return ResultType::INDETERMINATE;
        case HEADER_NAME:
            if (input == ':') { state_ = HEADER_VALUE; return ResultType::INDETERMINATE; }
            if (!is_char(input) || is_ctl(input) || is_tspecial(input)) { return ResultType::BAD; }
            req.headers.back().first.push_back(input);
            return ResultType::INDETERMINATE;
        case HEADER_VALUE:
            if (input == '\r') { state_ = EXPECTING_NEWLINE_2; return ResultType::INDETERMINATE; }
            if (is_ctl(input)) { return ResultType::BAD; }
            req.headers.back().second.push_back(input);
            return ResultType::INDETERMINATE;
        case EXPECTING_NEWLINE_2:
            if (input == '\n') { state_ = HEADER_LINE_START; return ResultType::INDETERMINATE; } return ResultType::BAD;
        case EXPECTING_NEWLINE_3:
            // If we get a final \n, we are done.
            return (input == '\n') ? ResultType::GOOD : ResultType::BAD;
        default: return ResultType::BAD;
        }
    }

    enum State {
        METHOD_START,
        METHOD,
        URI,
        HTTP_VERSION_H,
        HTTP_VERSION_T1,
        HTTP_VERSION_T2,
        HTTP_VERSION_P,
        HTTP_VERSION_SLASH,
        HTTP_VERSION_MAJOR,
        HTTP_VERSION_MINOR,
        EXPECTING_NEWLINE_1,
        HEADER_LINE_START,
        HEADER_LWS,
        HEADER_NAME,
        HEADER_VALUE,
        EXPECTING_NEWLINE_2,
        EXPECTING_NEWLINE_3
    } state_ = METHOD_START;

    static bool is_char(int c) { return c >= 0 && c <= 127; }
    static bool is_ctl(int c) { return (c >= 0 && c <= 31) || (c == 127); }
    static bool is_tspecial(int c) {
        switch (c) {
        case '(': case ')': case '<': case '>': case '@':
        case ',': case ';': case ':': case '\\': case '"':
        case '/': case '[': case ']': case '?': case '=':
        case '{': case '}': case ' ': case '\t': return true;
        default: return false;
        }
    }
    static bool is_digit(int c) { return c >= '0' && c <= '9'; }
};

#endif