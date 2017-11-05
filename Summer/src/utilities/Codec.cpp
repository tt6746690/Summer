
#include "Codec.h"

#include <string>

#include <sstream>  // hex transformation
#include <iomanip>  // stream manipulator 
#include <iostream>
#include <cassert>

namespace Summer
{

// Base64Codec

// Local Lookup tables, and compile time constants
static constexpr char base64_pad = '=';
static constexpr BYTE encode_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static constexpr BYTE decode_table[256] = {
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63, 52, 53, 54, 55, 56, 57,
    // `=`
    58, 59, 60, 61, 64, 64, 64, 0, 64, 64, 64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64,
    64, 64, 64, 64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
    40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64};

BYTE_STRING Base64Codec::encode(const std::string &in)
{
    const auto input = BYTE_STRING(in.begin(), in.end());
    return encode(input);
}

BYTE_STRING Base64Codec::encode(const BYTE_STRING &input)
{
    BYTE_STRING encoded;

    auto len = input.size();
    encoded.reserve(4 * ((len / 3) + (len % 3 > 0)));

    auto ptr = input.data();
    auto end = ptr + input.size();

    while (end - ptr >= 3)
    {
        encoded += encode_table[enc1(ptr)];
        encoded += encode_table[enc2(ptr)];
        encoded += encode_table[enc3(ptr)];
        encoded += encode_table[enc4(ptr)];
        ptr += 3;
    }

    switch (len % 3)
    {
        case 1:
        {
            encoded += encode_table[enc1(ptr)];
            encoded += encode_table[enc2(ptr)];
            encoded += base64_pad;
            encoded += base64_pad;
            break;
        }
        case 2:
        {
            encoded += encode_table[enc1(ptr)];
            encoded += encode_table[enc2(ptr)];
            encoded += encode_table[enc3(ptr)];
            encoded += base64_pad;
            break;
        }
        default:
            break;
    }

    return encoded;
}

std::pair<BYTE_STRING, bool> Base64Codec::decode(const BYTE_STRING input)
{
    BYTE_STRING decoded;

    if (input.size() % 4)
        return std::make_pair(decoded, false);

    BYTE_STRING chunk4b;
    chunk4b.reserve(4);
    auto start = chunk4b.data();

    auto ptr = input.begin();
    while (ptr < input.end())
    {
        chunk4b[0] = decode_table[*ptr];
        chunk4b[1] = decode_table[*(ptr + 1)];
        chunk4b[2] = decode_table[*(ptr + 2)];
        chunk4b[3] = decode_table[*(ptr + 3)];

        decoded += dec1(start);
        decoded += dec2(start);
        decoded += dec3(start);

        ptr += 4;
    }

    // remove trailing whitespace
    ptr = decoded.end();
    while (*(ptr - 1) == 0)
    {
        --ptr;
    }
    decoded.erase(ptr, input.end());

    return std::make_pair(decoded, true);
}

// Local helper functions

/**
 * @brief   Convert to/from hex string
 */
template <typename InputIter>
std::string to_hex_string(InputIter begin, InputIter end, bool insert_spaces = false)
{
    std::ostringstream ss;
    ss << std::setfill('0') << std::hex;
    while (begin != end)
    {
        ss << std::setw(2) << static_cast<uint32_t>(*begin++);
        if (insert_spaces && begin != end)
            ss << " ";
    }
    return ss.str();
}

BYTE_STRING from_hex_string(std::string &hex_string)
{
    std::stringstream converter;
    std::istringstream ss(hex_string);
    BYTE_STRING byte_array;

    std::string word;
    while (ss >> word)
    {
        BYTE temp;
        converter << std::hex << word;
        converter >> temp;
        byte_array += temp;
    }
    return byte_array;
}
std::ostream &operator<<(std::ostream &strm, const BYTE_STRING in)
{
    strm << to_hex_string(in.begin(), in.end());
    return strm;
}

// SHA256Codec

// first 32 bits of fractional parts of cube roots of first 64 prime numbers
static constexpr WORD K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
    0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
    0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
    0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
    0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
    0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

std::string SHA256Codec::digest(const BYTE_STRING &message)
{
    update(message);
    finish();
    return get_hash();
}

std::string SHA256Codec::digest(const std::string &message)
{
    return digest(BYTE_STRING(message.begin(), message.end()));
}

void SHA256Codec::update(const std::string &message)
{
    update(BYTE_STRING(message.begin(), message.end()));
}
void SHA256Codec::update(const BYTE_STRING &message)
{
    auto l = message.size();

    for (int i = 0; i < l; ++i)
    {
        blk_ += message[i];
        if (blk_.size() == 64)
        {
            transform();
            blk_.clear();
            assert(blk_.size() == 0);
            bit_length_ += 512;
        }
    }
    bit_length_ += blk_.size() * 8;
}

void SHA256Codec::finish()
{
    if (blk_.size() < 56)
    {
        blk_ += static_cast<uint8_t>(0x80);
        while (blk_.size() < 56)
            blk_ += static_cast<uint8_t>(0x00);
    }
    else
    {
        blk_ += static_cast<uint8_t>(0x80);
        while (blk_.size() < 64)
            blk_ += static_cast<uint8_t>(0x00);
        transform();
        blk_.clear();
        while (blk_.size() < 56)
            blk_ += static_cast<uint8_t>(0x00);
    }
    assert(blk_.size() == 56);

    // last 8 bytes of blk_ holds message length in bits
    for (int i = 0; i < 8; ++i)
    {
        blk_ += (bit_length_ >> (56 - 8 * i));
    }

    assert(blk_.size() == 64);
    transform();
}

void SHA256Codec::transform()
{
    WORD a, b, c, d, e, f, g, h, t1, t2, W[64];

    for (int i = 0, j = 0; i < 16; ++i, j += 4)
    {
        W[i] = (blk_[j] << 24) | (blk_[j + 1]) << 16 | (blk_[j + 2]) << 8 |
               (blk_[j + 3]);
    }
    for (int i = 16; i < 64; ++i)
    {
        W[i] = SIGLOW1(W[i - 2]) + W[i - 7] + SIGLOW0(W[i - 15]) + W[i - 16];
    }

    a = hash_[0];
    b = hash_[1];
    c = hash_[2];
    d = hash_[3];
    e = hash_[4];
    f = hash_[5];
    g = hash_[6];
    h = hash_[7];

    for (int i = 0; i < 64; ++i)
    {
        t1 = h + SIGUPP1(e) + CH(e, f, g) + K[i] + W[i];
        t2 = SIGUPP0(a) + MAJ(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    hash_[0] += a;
    hash_[1] += b;
    hash_[2] += c;
    hash_[3] += d;
    hash_[4] += e;
    hash_[5] += f;
    hash_[6] += g;
    hash_[7] += h;
}

void SHA256Codec::print_hash(std::string msg)
{
    std::ios::fmtflags f(std::cout.flags());
    std::cout << msg << ": ";
    for (int i = 0; i < 8; ++i)
    {
        std::cout << std::setw(8) << std::setfill('0') << std::hex << hash_[i]
                  << " ";
    }
    std::cout << std::endl;
    std::cout.flags(f);
}

std::string SHA256Codec::get_hash()
{
    std::ostringstream ss;
    for (int i = 0; i < 8; ++i)
        ss << std::setw(8) << std::setfill('0') << std::hex << hash_[i];
    return ss.str();
}
}