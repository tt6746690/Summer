#include <iterator>
#include <utility>
#include "catch.hpp"

#include "RequestParser.h"
#include "Message.h"
#include "Utils.h"

using namespace std;
using namespace Theros;



TEST_CASE("Message")
{
    SECTION("HttpVersion")
    {
        auto test_httpversion = [](HttpVersion v, string s){
            REQUIRE(version_as_string(v) == s);
        };
        test_httpversion(HttpVersion::zero_nine, "HTTP/0.9");
        test_httpversion(HttpVersion::one_zero,  "HTTP/1.0");
        test_httpversion(HttpVersion::one_one,   "HTTP/1.1");
        test_httpversion(HttpVersion::two_zero,  "HTTP/2.0");
    }

    SECTION("Message")
    {
        using DefaultMessage = Message<>;
        using TestHeader = DefaultMessage::Header;
        using TestHeadersType = vector<pair<string, string>>;
        DefaultMessage msg;

        SECTION("Init")
        {
            REQUIRE(msg.headers.size() == 0);
            REQUIRE(msg.version == HttpVersion::undetermined);
        }


        SECTION("FindHeader")
        {
            auto test_find_header = [](DefaultMessage msg,
                const TestHeadersType& expect_in, const TestHeadersType& expect_notin) {
                for (auto& p: expect_in) {
                    TestHeader h;
                    h.name = p.first;
                    h.value = p.second;
                    msg.headers.push_back(h);
                }

                for (auto& p: expect_in) {
                    REQUIRE(msg.FindHeader(p.first) == p.second);
                }

                for (auto& p: expect_notin) {
                    REQUIRE(msg.FindHeader(p.first) == "");
                }
            };

            test_find_header(msg, { {"egg", "tomato"}, {"quill", "ink"} },
                { {"oak", "maple"}, {"beaver", "canada"} });
        }

        SECTION("SetHeader")
        {
            auto test_set_uniq_headers = [](DefaultMessage msg, const TestHeadersType& v)
            {
                for(auto& p: v) {
                    TestHeader h;
                    h.name = p.first;
                    h.value = p.second;
                    msg.SetHeader(h);
                    REQUIRE(msg.headers.back().name == p.first);
                    REQUIRE(msg.headers.back().value == p.second);
                    REQUIRE(msg.FindHeader(h.name) == h.value);
                }
                REQUIRE(msg.headers.size() == v.size());
            };

            test_set_uniq_headers(msg, {
                {"foo", "bar"},
                {"baz", "bya"}
            });

            auto test_reset_sam_headers = [](DefaultMessage msg, const TestHeadersType& v, size_t expect_size)
            {
                for(auto& p: v) {
                    TestHeader h;
                    h.name = p.first;
                    h.value = p.second;
                    msg.SetHeader(h);
                    REQUIRE(msg.FindHeader(h.name) == h.value);
                }
                REQUIRE(msg.headers.size() == expect_size);
            };

            test_reset_sam_headers(msg, {
                {"foo", "bar"},
                {"foo", "rab"}
            }, 1);
        }
    }

    SECTION("Uri") 
    {
        auto test_uri_as_string = [](vector<string> c, string expect){
            REQUIRE(c.size() == 6);
            Uri uri;
            uri.scheme      = c[0];
            uri.host        = c[1];
            uri.port        = c[2];
            uri.abs_path    = c[3];
            uri.query       = c[4];
            uri.fragment    = c[5];
            REQUIRE(expect == uri_as_string(uri));
        };
        test_uri_as_string({"https", "www.youtube.com", "443", "/feed/trending", "gl=CA", "search"},
            "https://www.youtube.com:443/feed/trending?gl=CA#search");

        test_uri_as_string({"https", "www.youtube.com", "", "/feed/trending", "gl=CA", ""},
            "https://www.youtube.com/feed/trending?gl=CA");
    }


    SECTION("Request") 
    {
        Request req;
        REQUIRE(req.method == RequestMethod::UNDETERMINED);

        SECTION("Conversions")
        {
            auto test_conversions = [](const char* m_cstr, RequestMethod m) {
                REQUIRE(string(m_cstr) == request_method_as_string(m));
                REQUIRE(m == request_method_from_cstr(m_cstr));
            };

            test_conversions("GET", RequestMethod::GET);
            test_conversions("POST", RequestMethod::POST);
            test_conversions("PATCH", RequestMethod::PATCH);
        }
    }

    SECTION("Response")
    {
        Response res;
        REQUIRE(res.status_code == StatusCode::OK);

        SECTION("Conversions")
        {
            auto test_conversions = [](StatusCode c, int c_int, const char* reason) {
                REQUIRE(status_code_as_int(c) == c_int);
                REQUIRE(status_code_as_reason(c) == reason);
                REQUIRE(status_code_from_int(c_int) == c);
            };

            test_conversions(StatusCode::OK, 200, "OK");
            test_conversions(StatusCode::Not_Found, 404, "Not Found");
        }

    }
}
