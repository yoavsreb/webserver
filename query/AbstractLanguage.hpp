#ifndef _QUERY_ABSTRACT_LANGUAGE_HPP
#define _QUERY_ABSTRACT_LANGUAGE_HPP

#include <boost/variant.hpp>
#include <string>

enum class TokenType {
    NONE = 0,
    NUMBER,
    PLUS,
    MULTIPLY,
    LPARAN,
    RPARAN,
    UUID
};

struct Token {
    TokenType type;
    boost::variant<int, std::string> value;

    template<typename T>
    Token(TokenType t, const T& v) : type(t), value(v) {}

    Token() : type(TokenType::NONE), value(-1) {}
};

#endif
