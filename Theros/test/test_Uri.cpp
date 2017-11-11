#include "catch.hpp"
#include <utility>
#include <stdexcept>
#include <string>

#include "Uri.h"

using namespace Theros;

TEST_CASE("uri encoding/decoding", "[Uri]")
{


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

}
