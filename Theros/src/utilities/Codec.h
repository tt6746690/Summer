#ifndef __CODEC_H__
#define __CODEC_H__

// reference http://web.mit.edu/freebsd/head/contrib/wpa/src/utils/base64.c

#include <cstdint>
#include <string>
#include <utility>

namespace Theros
{

using BYTE = uint8_t;
using WORD = uint32_t;
using BYTE_STRING = std::basic_string<BYTE>;
using WORD_STRING = std::basic_string<WORD>;

struct Base64Codec
{
  /**@brief   Encode data to base64 encoding
   *          3 bytes (24 bits) -> 4 base64 character (6 bits)
   */
  static BYTE_STRING encode(const std::string &in);
  static BYTE_STRING encode(const BYTE_STRING &input);
  /**@brief   Decode data from base64 encoding
   *          4 base64 character (6 bits) -> 3 bytes (24 bits)
   */
  static std::pair<BYTE_STRING, bool> decode(const BYTE_STRING input);

private:
  /**
   * @brief   Converts from a 3 1b (BYTE) to 4 1b base64
   */
  static constexpr BYTE enc1(const BYTE *pos) { return (BYTE)(pos[0] >> 2); }
  static constexpr BYTE enc2(const BYTE *pos) { return (BYTE)((pos[0] & 0x03) << 4) | (pos[1] >> 4); }
  static constexpr BYTE enc3(const BYTE *pos) { return (BYTE)((pos[1] & 0x0f) << 2) | (pos[2] >> 6); }
  static constexpr BYTE enc4(const BYTE *pos) { return (BYTE)(pos[2] & 0x3f); }
  /**
   * @brief   Converts from 4 1b base 64 (i.e. 0~63) to a 3 1b (BYTE)
   */
  static constexpr BYTE dec1(const BYTE *pos) { return ((pos[0] << 2) | (pos[1] >> 4)); }
  static constexpr BYTE dec2(const BYTE *pos) { return ((pos[1] << 4) | (pos[2] >> 2)); }
  static constexpr BYTE dec3(const BYTE *pos) { return ((pos[2] << 6) | pos[3]); }
};

// reference : http://csrc.nist.gov/publications/fips/fips180-4/fips-180-4.pdf

struct SHA256Codec
{
  /**
   * @brief   Consumes entire message at once
   */
  std::string digest(const BYTE_STRING &message);
  std::string digest(const std::string &message);
  /**
   * @brief   Update hash_ by iterating over message blocks of M
   */
  void update(const std::string &message);
  void update(const BYTE_STRING &message);
  /**@brief   Pad message M,
   *          parse into message blks, and
   *          setting initial hash value H
   */
  void finish();
  /**
   * @brief   Generate digests, in hexidecimal string
   */
  std::string get_hash();
  void print_hash(std::string msg);

private:
  BYTE_STRING blk_;
  WORD hash_[8] = {
      0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
      0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19,
  };
  int64_t bit_length_ = 0;
  /**
   * @brief   Use a message schedule of 64 32bit words to
   *          transform hash_ to next state
   */
  void transform();

  static constexpr int BLOCK_SIZE = (512 / 8); // 64 byte blocks
  constexpr static int DIGEST_SIZE = 32;       // 32 bytes = 8 32bit words
  // transformations
  static constexpr WORD ROTL(WORD x, int n) { return (x << n) | (x >> (32 - n)); };
  static constexpr WORD ROTR(WORD x, int n) { return (x >> n) | (x << (32 - n)); };
  static constexpr WORD SHR(WORD x, int n) { return (x >> n); }
  static constexpr WORD CH(WORD x, WORD y, WORD z) { return (x & y) ^ (~x & z); }
  static constexpr WORD MAJ(WORD x, WORD y, WORD z) { return (x & y) ^ (x & z) ^ (y & z); }
  // Hash functions
  static constexpr WORD SIGUPP0(WORD x) { return ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22); }
  static constexpr WORD SIGUPP1(WORD x) { return ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25); }
  static constexpr WORD SIGLOW0(WORD x) { return ROTR(x, 7) ^ ROTR(x, 18) ^ SHR(x, 3); }
  static constexpr WORD SIGLOW1(WORD x) { return ROTR(x, 17) ^ ROTR(x, 19) ^ SHR(x, 10); }
};

} // namespace Theros
#endif // __CODEC_H__
