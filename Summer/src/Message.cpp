#include "Message.h"
#include "Constants.h"

#include <ostream>
#include <cassert>
#include <algorithm>


namespace Summer
{

std::string Message::version(int major, int minor)
{
    return "HTTP/" + std::to_string(major) + "." + std::to_string(minor);
}

void Message::build_header_name(char c)
{
    assert(headers_.size() != 0);
    header_name(headers_.back()).push_back(c);
}
void Message::build_header_value(char c)
{
    assert(headers_.size() != 0);
    header_value(headers_.back()).push_back(c);
}

Message::HeaderNameType& Message::header_name(HeaderType &header)
{
    return std::get<0>(header);
}
Message::HeaderValueType& Message::header_value(HeaderType &header)
{
    return std::get<1>(header);
}

std::pair<Message::HeaderValueType, bool> Message::get_header(HeaderNameType name)
{
    HeaderValueType val{};
    bool valid = false;

    for (auto &header : headers_)
    {
        if (header_name(header) == name)
            val = header_value(header), valid = true;
    }
    return std::make_pair(val, valid);
}

std::string Message::flatten_header() const
{
    std::string headers_flat;
    for (auto &header : headers_)
        headers_flat += header.first + ": " + header.second + CRLF;
    return headers_flat + CRLF;
}

void Message::set_header(HeaderType header)
{
    auto found = find_if(headers_.begin(), headers_.end(),
                         [&](auto &h) {
                             return h.first == header_name(header);
                         });
    if (found != headers_.end())
        *found = header;
    else
        headers_.push_back(header);
}

void Message::unset_header(HeaderNameType name)
{
    auto end = std::remove_if(headers_.begin(), headers_.end(),
                              [&](auto &header) {
                                  return header.first == name;
                              });
    headers_.erase(end, headers_.end());
}

int Message::content_length()
{
    HeaderValueType val = "";
    bool found;
    std::tie(val, found) = get_header("Content-Length");

    if (found)
        return std::atoi(val.c_str());

    set_header({"Content-Length", "0"});
    return 0;
}

void Message::content_length(int length)
{
    set_header({"Content-Length", std::to_string(length)});
}

Message::HeaderValueType Message::content_type()
{
    HeaderValueType val = "";
    bool found;
    std::tie(val, found) = get_header("Content-Type");

    if (found)
        return val;
    else
        set_header({"Content-Type", ""});
    return "";
}

void Message::content_type(HeaderValueType value)
{
    set_header({"Content-Type", value});
}

std::ostream& operator<<(std::ostream& os, Message::HeaderType& header)
{
    return os << Message::header_name(header) << ": " << Message::header_value(header) << std::endl;
}
}