#include "catch.hpp"
#include "Codec.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>


using namespace Http;

auto test_base64(std::pair<std::string, std::string> in_out) -> void {

  auto in = std::get<0>(in_out);
  auto out = std::get<1>(in_out);

  auto input = BYTE_STRING(in.begin(), in.end());
  auto output = BYTE_STRING(out.begin(), out.end());

  // encoding
  auto encoded = Base64Codec::encode(input);
  REQUIRE(encoded == output);
  auto str_encoded = Base64Codec::encode(in);
  REQUIRE(str_encoded == output);

  // decoding
  BYTE_STRING decoded;
  bool success;
  std::tie(decoded, success) = Base64Codec::decode(output);

  // std::cout << "in: [" << output << "] decoded: [" << decoded << "] == ["
  //           << input << "]" << std::endl;
  REQUIRE(success == true);
  REQUIRE(decoded.size() == input.size());
  REQUIRE(decoded == input);
}

TEST_CASE("base64 encoding/decoding", "[Codec]") {

  SECTION("base64 encoding") {
    test_base64({"pleasure.", "cGxlYXN1cmUu"});
    test_base64({"leasure.", "bGVhc3VyZS4="});
    test_base64({"easure.", "ZWFzdXJlLg=="});
    test_base64({"asure.", "YXN1cmUu"});
    test_base64({"sure.", "c3VyZS4="});
  }
}

auto test_sha256(std::pair<std::string, std::string> in_out) -> void {

  auto in = std::get<0>(in_out);
  auto out = std::get<1>(in_out);
  auto message = BYTE_STRING(in.begin(), in.end());

  auto digest = SHA256Codec().digest(message);

  // std::cout << "digest: " << digest << std::endl;
  // std::cout << "correc: " << out << std::endl;

  REQUIRE(digest == out);
}

TEST_CASE("sha256 digest", "[Codec]") {

  SECTION("sha256 digest") {
    test_sha256(
        {"1234567",
         "8bb0cf6eb9b17d0f7d22b456f121257dc1254e1f01665370476383ea776df414"});
    test_sha256(
        {"helloworld",
         "936a185caaa266bb9cbe981e9e05cb78cd732b0b3280eb944412bb6f8f8f07af"});

    test_sha256(
        {"SHA-2 (Secure Hash Algorithm 2) is a set of cryptographic hash "
         "functions designed by the United States National Security Agency "
         "(NSA).[3] Cryptographic hash functions are mathematical operations "
         "run on digital data; by comparing the computed hash (the output from "
         "execution of the algorithm) to a known and expected hash value, a "
         "person can determine the data's integrity. For example, computing "
         "the hash of a downloaded file and comparing the result to a "
         "previously published hash result can show whether the download has "
         "been modified or tampered with.[4] A key aspect of cryptographic "
         "hash functions is their collision resistance: nobody should be able "
         "to find two different input values that result in the same hash "
         "output.",
         "cfcc61b181e8c87e765f8a17913258394088eab1976b110f9bf0bb5388e4304b"});
  }
}