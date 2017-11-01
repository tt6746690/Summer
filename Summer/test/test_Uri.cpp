#include "catch.hpp"
#include <utility>
#include <stdexcept>
#include <string>

#include "Uri.h"

using namespace Http;

TEST_CASE("uri encoding/decoding", "[Uri]")
{

    std::string url;

    SECTION("ctohex")
    {
        REQUIRE(ctohex('#') == "23");
        REQUIRE(ctohex('%') == "25");
    }

    SECTION("encoding")
    {
        url = u8"François";
        REQUIRE(Uri::urlencode(url) == "Fran%C3%A7ois");
    }

    SECTION("decoding")
    {
        url = "a%20space";
        REQUIRE(Uri::urldecode(url) == u8"a space");

        url = "Fran%C3%A7ois";
        REQUIRE(Uri::urldecode(url) == u8"François");
    }

    SECTION("instance decoding")
    {
        // https://zh.wikipedia.org/wiki/%E7%99%BE%E5%88%86%E5%8F%B7%E7%BC%96%E7%A0%81

        Uri uri;
        uri.scheme_ = "https";
        uri.host_ = "zh.wikipedia.org";
        uri.abs_path_ = "/wiki/%E7%99%BE%E5%88%86%E5%8F%B7%E7%BC%96%E7%A0%81";

        uri.decode();

        REQUIRE(uri.scheme_ == "https");
        REQUIRE(uri.host_ == "zh.wikipedia.org");
        REQUIRE(uri.abs_path_ == "/wiki/百分号编码");
    }

    SECTION("make query")
    {
        auto query = Uri::make_query("foo=bar&a=d,s,d");
        REQUIRE_NOTHROW(query.at("foo"));
        REQUIRE(query["foo"] == "bar");
        REQUIRE_NOTHROW(query.at("a"));
        REQUIRE(query["a"] == "d,s,d");
    }
}
