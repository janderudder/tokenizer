#include "tokenizer/Tokenizer.hpp"
#include <algorithm>
#include <cassert>



Tokenizer::Tokenizer(Separators const& sep) noexcept:
    Tokenizer {sep.single, sep.maybe_double}
{
}



Tokenizer::Tokenizer(
    std::string_view const single_separators,
    std::string_view const maybe_double_separator_tokens
) noexcept:
    m_separator_tokens {std::move(single_separators)},
    m_single_sep_end   {single_separators.size()}
{
    m_separator_tokens.insert(m_separator_tokens.size(),
        std::move(maybe_double_separator_tokens));
}



////////////////////////////////////////////////////////////////////////////////
void Tokenizer::set_tab_size(unsigned tabsize)
{
    m_tab_size = tabsize;
}



////////////////////////////////////////////////////////////////////////////////
auto Tokenizer::operator()(std::string_view input) const -> output_type
{
    output_type tokens;

    size_t  index  = 0,
            line   = 0,
            column = 0;

    auto const store_token =
        [&tokens, &line, &column](std::string_view const token_value) {
            tokens.push_back({token_value, line, column});
        };

    auto const input_size = input.size();

    while (index < input_size)
    {
        auto const remaining = input.substr(index);

        auto const next_discardable_sep =
            remaining.find_first_of(m_discardable_separators);

        auto const next_sep_token =
            remaining.find_first_of(m_separator_tokens);

        auto const next = std::min(next_discardable_sep, next_sep_token);

        if (next == 0) // separator is right where we look, treat it
        {
            if (0 == next_discardable_sep)
            {
                auto const discardable_end
                    = remaining.find_first_not_of(m_discardable_separators);

                if (discardable_end == remaining.npos) {
                    break;
                }

                index += discardable_end;

                auto const [line_offs, column_offs] =
                    _handle_discardable(remaining.substr(0, discardable_end));

                if (line_offs == 0) {
                    column += column_offs;
                }
                else {
                    line += line_offs;
                    column = column_offs;
                }
            }
            else // if (0 == next_sep_token)
            {
                auto const token_length = _get_separator_length(remaining);
                store_token(remaining.substr(0, token_length));
                index  += token_length;
                column += token_length;
            }
        }
        // the next separator is not right there at 0, so store the token
        // before that index. But it may be the only token (next == npos).
        else if (next == remaining.npos)
        {
            store_token(input.substr(index));
            break;
        }
        else
        {
            store_token(remaining.substr(0, next));
            column += next;
            index  += next;
        }
    }


    return tokens;
}



////////////////////////////////////////////////////////////////////////////////
auto Tokenizer::_get_separator_length(std::string_view const remaining) const
    -> size_t
{
    auto const separator_character = remaining[0];

    auto const may_be_double =
        !(m_separator_tokens.find(separator_character) < m_single_sep_end);

    if (!may_be_double
        || remaining.size() == 1
        || remaining[1] != separator_character)
    {
        return 1;
    }
    return 2;
}



auto Tokenizer::_handle_discardable(std::string_view const discardable_input)
    const -> line_column_pair_t
{
    size_t line = 0, column = 0;
    for (auto const c : discardable_input)
    {
        switch (c) {
            case '\n' : ++line; column=0;           break;
            case '\t' : column+=m_tab_size;         break;
            case '\v' : ++line; column+=m_tab_size; break;
            default   : ++column;
        }
    }
    return {line, column};
}
