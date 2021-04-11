#pragma once
#include <string_view>



struct Token
{
    static constexpr auto nil = size_t(-1);

    std::string_view value;
    size_t           line;
    size_t           column;

    template <typename Str = std::string_view>
    constexpr Token(Str&& str ={}, size_t =nil, size_t =nil) noexcept;

    static constexpr bool value_eq(Token const&, Token const&) noexcept;
};



// inline impl.
////////////////////////////////////////////////////////////////////////////////
template <typename Str>
constexpr Token::Token(Str&& str, size_t lin, size_t col) noexcept:
    value  {std::forward<Str>(str)},
    line   {lin},
    column {col}
{
}



constexpr bool Token::value_eq(Token const& l, Token const& r) noexcept
{
    return l.value == r.value;
}
