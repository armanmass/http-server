#include <gtest/gtest.h>
#include <http_parser.hpp>
#include <http_request.hpp>

TEST(ParserTest, SimpleGETRequest){
    HttpRequestParser parser;
    HttpRequest req;
    std::string request_string = "GET /index.html "
                                 "HTTP/1.1\r\nHost: "
                                 "www.test.com\r\n\r\n";
    auto result = parser.parse(req, request_string.begin(), request_string.end());
    ASSERT_EQ(std::get<0>(result), HttpRequestParser::ResultType::GOOD);
    EXPECT_EQ(req.method, "GET");
    EXPECT_EQ(req.uri, "/index.html");
    EXPECT_EQ(req.http_version_major, 1);
    EXPECT_EQ(req.http_version_minor, 1);

    auto host_header = std::find_if(req.headers.begin(), req.headers.end(),
                        [](const auto& h) { return h.first == "Host"; });
    ASSERT_NE(host_header, req.headers.end());
    EXPECT_EQ(host_header->second, " www.test.com");
}