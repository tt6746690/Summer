#include <iterator>
#include <utility>
#include "catch.hpp"

#include "Message.h"
#include "Utils.h"

using namespace std;
using namespace Theros;


TEST_CASE("Enum StatusCode", "[Response]")
{
    Response res;

    SECTION("StatusCode enum to int")
    {
        REQUIRE(Response::status_code_to_int(StatusCode::Found) == 302);
    }

    SECTION("StatusCode enum to reason phrase")
    {
        const char *OK = Response::status_code_to_reason(StatusCode::OK);
        REQUIRE(OK == "OK");

        std::string Bad_Request = Response::status_code_to_reason(StatusCode::Bad_Request);
        REQUIRE(Bad_Request == "Bad Request");
    }
}



TEST_CASE("url percent encoding/decoding")
{

    SECTION("instance decoding")
    {
        // https://zh.wikipedia.org/wiki/%E7%99%BE%E5%88%86%E5%8F%B7%E7%BC%96%E7%A0%81

        Uri uri;
        uri.scheme = "https";
        uri.host = "zh.wikipedia.org";
        uri.abs_path = "/wiki/%E7%99%BE%E5%88%86%E5%8F%B7%E7%BC%96%E7%A0%81";

        uri.decode();

        REQUIRE(uri.scheme == "https");
        REQUIRE(uri.host == "zh.wikipedia.org");
        REQUIRE(uri.abs_path == "/wiki/百分号编码");
    }

}


TEST_CASE("Message::Manipulate header member", "[Message]")
{
    Message msg;
    REQUIRE(msg.headers.size() == 0);

    msg.headers.emplace_back();
    REQUIRE(msg.headers.size() == 1);

    // SECTION("build_header_name")
    // {
    //     msg.build_header_name('k');
    //     REQUIRE(msg.headers.back().first == "k");

    //     msg.build_header_name('e');
    //     msg.build_header_name('y');
    //     REQUIRE(msg.headers.back().first == "key");

    //     msg.build_header_value('v');
    //     msg.build_header_value('a');
    //     REQUIRE(msg.headers.back().second == "va");

    //     msg.build_header_value('l');
    //     msg.build_header_value('u');
    //     msg.build_header_value('e');
    //     REQUIRE(msg.headers.back().second == "value");
    // }

    SECTION("manipulate headers")
    {
        msg.set_header({"foo", "bar"});
        msg.set_header({"bar", "baz"});
        REQUIRE(msg.headers.size() == 3);

        SECTION("get_header")
        {
            std::string val;
            bool found;

            std::tie(val, found) = msg.get_header("foo");
            REQUIRE(val == "bar");
            REQUIRE(found == true);

            std::tie(val, found) = msg.get_header("not_in_header");
            REQUIRE(found == false);
        }

        SECTION("set_header")
        {
            msg.set_header({"foo", "barbar"});
            REQUIRE(msg.headers.size() == 3);

            auto found = std::find_if(msg.headers.begin(), msg.headers.end(), [](auto &header) {
                return header.first == "foo";
            });
            REQUIRE(found != msg.headers.end());
            REQUIRE(found->second == "barbar");
        }

        SECTION("unset_header")
        {
            msg.unset_header("foo");
            REQUIRE(msg.headers.size() == 2);

            auto found = std::find_if(msg.headers.begin(), msg.headers.end(), [](auto &header) {
                return header.first == "foo";
            });
            REQUIRE(found == msg.headers.end());
        }
    }
}
