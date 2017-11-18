#include <iterator>
#include <algorithm>
#include <map>
#include <string>
#include "catch.hpp"

#include "RequestParser.h"
#include "Message.h"
#include "Constants.h"

using namespace Theros;

TEST_CASE("Uri", "[RequestParser]")
{
    RequestParser parser;
    REQUIRE(parser.state_ == static_cast<RequestParser::State>(1));

    Request req;
    REQUIRE(req.method == RequestMethod::UNDETERMINED);
    REQUIRE(req.uri.state_ == UriState::uri_start);

    std::string payload;

    SECTION("good full uri")
    {
        payload = "POST http://abc.com:80/~smith/home.html#footer HTTP/1.0\r\n";
        parser.parse(req, std::begin(payload), std::end(payload));
        REQUIRE(req.uri.scheme == "http");
        REQUIRE(req.uri.host == "abc.com");
        REQUIRE(req.uri.port == "80");
        REQUIRE(req.uri.abs_path == "~smith/home.html");
        REQUIRE(req.uri.fragment == "footer");
    }
    SECTION("good abs path")
    {
        payload = "POST /~smith/home.html#footer HTTP/1.0\r\n";
        parser.parse(req, std::begin(payload), std::end(payload));
        REQUIRE(req.uri.scheme == "");
        REQUIRE(req.uri.host == "");
        REQUIRE(req.uri.port == "");
        REQUIRE(req.uri.abs_path == "/~smith/home.html");
        REQUIRE(req.uri.fragment == "footer");
    }
}

TEST_CASE("Overall", "[RequestParser]")
{
    RequestParser parser;
    REQUIRE(parser.state_ == static_cast<RequestParser::State>(1));
    
    Request req;
    REQUIRE(req.method == RequestMethod::UNDETERMINED);
    REQUIRE(req.uri.state_ == UriState::uri_start);
    REQUIRE(req.headers.size() == 0);
    REQUIRE(req.body == "");

    std::string payload;

    SECTION("parsing good inputs")
    {
        payload = "GET /hi HTTP/1.0\r\n"
                  "Host: 127.0.0.1:8888\r\n"
                  "User-Agent: curl/7.43.0\r\n"
                  "Accept: */*\r\n"
                  "\r\n";

        parser.parse(req, std::begin(payload), std::end(payload));

        REQUIRE(req.method == RequestMethod::GET);
        REQUIRE(req.uri.abs_path == "/hi");
        REQUIRE(req.version_major == 1);
        REQUIRE(req.version_minor == 0);
        REQUIRE(req.headers.size() == 3);

        std::map<std::string, std::string> headers{
            {"Host", "127.0.0.1:8888"},
            {"User-Agent", "curl/7.43.0"},
            {"Accept", "*/*"}};

        for (auto header : headers)
        {
            auto found = find_if(
                req.headers.begin(), req.headers.end(),
                [header](auto built_header) {
                    return header.first == built_header.first;
                });
            REQUIRE(found != req.headers.end());
            REQUIRE(found->first == header.first);
            REQUIRE(found->second == header.second);
        }
    }
}

TEST_CASE("Method", "[RequestParser]")
{

    RequestParser parser;
    REQUIRE(parser.state_ == static_cast<RequestParser::State>(1));
    
    Request req;
    REQUIRE(req.method == RequestMethod::UNDETERMINED);
    REQUIRE(req.uri.state_ == UriState::uri_start);

    std::string payload;

    SECTION("post")
    {
        payload = "POST /hi HTTP/1.0\r\n";
        parser.parse(req, std::begin(payload), std::end(payload));
        REQUIRE(req.method == RequestMethod::POST);
    }

    SECTION("patch")
    {
        payload = "PATCH /hi HTTP/1.0\r\n";
        parser.parse(req, std::begin(payload), std::end(payload));
        REQUIRE(req.method == RequestMethod::PATCH);
    }
    SECTION("connect")
    {
        payload = "CONNECT /hi HTTP/1.0 \r\n";
        parser.parse(req, std::begin(payload), std::end(payload));
    REQUIRE(req.method == RequestMethod::CONNECT);
        REQUIRE(req.version_minor == 0);
    }
}