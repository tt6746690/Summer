#include <iterator>
#include <algorithm>
#include <map>
#include <string>
#include "catch.hpp"

#include "RequestParser.h"
#include "Request.h"
#include "Constants.h"

using namespace Http;

TEST_CASE("Uri", "[RequestParser]")
{
    RequestParser parser;
    REQUIRE(parser.state_ == RequestParser::State::req_start);

    Request req;
    REQUIRE(req.method_ == RequestMethod::UNDETERMINED);
    REQUIRE(req.uri_.state_ == UriState::uri_start);

    std::string payload;

    SECTION("good full uri")
    {
        payload = "POST http://abc.com:80/~smith/home.html#footer HTTP/1.0\r\n";
        parser.parse(req, std::begin(payload), std::end(payload));
        REQUIRE(req.uri_.scheme_ == "http");
        REQUIRE(req.uri_.host_ == "abc.com");
        REQUIRE(req.uri_.port_ == "80");
        REQUIRE(req.uri_.abs_path_ == "~smith/home.html");
        REQUIRE(req.uri_.fragment_ == "footer");
    }
    SECTION("good abs path")
    {
        payload = "POST /~smith/home.html#footer HTTP/1.0\r\n";
        parser.parse(req, std::begin(payload), std::end(payload));
        REQUIRE(req.uri_.scheme_ == "");
        REQUIRE(req.uri_.host_ == "");
        REQUIRE(req.uri_.port_ == "");
        REQUIRE(req.uri_.abs_path_ == "/~smith/home.html");
        REQUIRE(req.uri_.fragment_ == "footer");
    }
}

TEST_CASE("Overall", "[RequestParser]")
{
    RequestParser parser;
    REQUIRE(parser.state_ == RequestParser::State::req_start);

    Request req;
    REQUIRE(req.method_ == RequestMethod::UNDETERMINED);
    REQUIRE(req.uri_.state_ == UriState::uri_start);
    REQUIRE(req.headers_.size() == 0);
    REQUIRE(req.body_ == "");

    std::string payload;

    SECTION("parsing good inputs")
    {
        payload = "GET /hi HTTP/1.0\r\n"
                  "Host: 127.0.0.1:8888\r\n"
                  "User-Agent: curl/7.43.0\r\n"
                  "Accept: */*\r\n"
                  "\r\n";

        parser.parse(req, std::begin(payload), std::end(payload));

        REQUIRE(req.method_ == RequestMethod::GET);
        REQUIRE(req.uri_.abs_path_ == "/hi");
        REQUIRE(req.version_major_ == 1);
        REQUIRE(req.version_minor_ == 0);
        REQUIRE(req.headers_.size() == 3);

        std::map<std::string, std::string> headers{
            {"Host", "127.0.0.1:8888"},
            {"User-Agent", "curl/7.43.0"},
            {"Accept", "*/*"}};

        for (auto header : headers)
        {
            auto found = find_if(
                req.headers_.begin(), req.headers_.end(),
                [header](auto built_header) {
                    return header.first == built_header.first;
                });
            REQUIRE(found != req.headers_.end());
            REQUIRE(found->first == header.first);
            REQUIRE(found->second == header.second);
        }
    }
}

TEST_CASE("Method", "[RequestParser]")
{

    RequestParser parser;
    REQUIRE(parser.state_ == RequestParser::State::req_start);

    Request req;
    REQUIRE(req.method_ == RequestMethod::UNDETERMINED);
    REQUIRE(req.uri_.state_ == UriState::uri_start);

    std::string payload;

    SECTION("post")
    {
        payload = "POST /hi HTTP/1.0\r\n";
        parser.parse(req, std::begin(payload), std::end(payload));
        REQUIRE(req.method_ == RequestMethod::POST);
    }

    SECTION("patch")
    {
        payload = "PATCH /hi HTTP/1.0\r\n";
        parser.parse(req, std::begin(payload), std::end(payload));
        REQUIRE(req.method_ == RequestMethod::PATCH);
    }
    SECTION("connect")
    {
        payload = "CONNECT /hi HTTP/1.0 \r\n";
        parser.parse(req, std::begin(payload), std::end(payload));
        REQUIRE(req.method_ == RequestMethod::CONNECT);
        REQUIRE(req.version_minor_ == 0);
    }
}