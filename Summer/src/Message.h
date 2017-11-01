#ifndef HEADER_H
#define HEADER_H

#include <list>
#include <ostream>
#include <string>
#include <utility> // pair
#include <vector>

namespace Http {

class Message {
public:
  using HeaderNameType = std::string;
  using HeaderValueType = std::string;
  using HeaderType = std::pair<HeaderNameType, HeaderValueType>;

  /**
   * @brief   appends a char to name/value of last header in headers
   *
   * @pre headers_ must be nonempty
   */
  void build_header_name(char c);
  void build_header_value(char c);

  /**
   * @brief   Gets header with given name
   */
  auto get_header(HeaderNameType name) -> std::pair<HeaderValueType, bool>;
  /**
   * @brief   Concatenates header key:value pair
   */
  auto flatten_header() const -> std::string;
  /**
   * @brief   Sets header with given name and value
   *
   * Overwrites existing header if name matches,
   * otherwise appends header to end of headers_
   */
  void set_header(HeaderType);
  /**
   * @brief   Removes header with given name
   */
  void unset_header(HeaderNameType name);

  /**
     * @brief   Gets/Sets commonly used headers
     */
  auto content_length() -> int;
  void content_length(int length);
  auto content_type() -> HeaderValueType;
  void content_type(HeaderValueType value);

  int version_major_;
  int version_minor_;
  std::vector<HeaderType> headers_;
  std::string body_;

public:
  /**
   * @brief   Return HTTP version
   */
  static auto version(int major, int minor) -> std::string;

  /**
   * @brief   Given a header, return its name/value
   */
  static auto header_name(HeaderType &header) -> HeaderNameType &;
  static auto header_value(HeaderType &header) -> HeaderValueType &;

  friend auto operator<<(std::ostream &strm, HeaderType) -> std::ostream &;
};
}
#endif
