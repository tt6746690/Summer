#ifndef __QUERYPARSER_H__
#define __QUERYPARSER_H__

#include <string>
#include <unordered_map>

#include "../Message.h"
#include "../Router.h"

namespace Theros
{

/**
 * @brief   Convert a query string to a map of key-value pairs, 
 *          the kv map is accessible from Context
 * @note    query map is cleared, ideally just use once in a cycle
 */
class QueryParser : public Handler
{

  public:
    QueryParser() { populate_uri_query(); }

  private:
    void populate_uri_query()
    {
        auto queryparser_handler = [](Context & ctx) 
        {
            constexpr char tok_and = '&';
            constexpr char tok_equal = '=';

            std::string query = ctx.req.uri.query;
            query += tok_and;

            std::size_t pos = 0;
            std::string token, key, value;

            while ((pos = query.find(tok_and)) != std::string::npos) {
                token = query.substr(0, pos);
                std::tie(key, value) = split(token, tok_equal);
                ctx.req.uri_query.insert({key, value});
                query.erase(0, pos + 1);
            }
        };

       append(queryparser_handler);
    }
};

} // namespace Theros
#endif // __QUERYPARSER_H__