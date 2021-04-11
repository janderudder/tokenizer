#pragma once
#include "tokenizer/Token.hpp"
#include <string>
#include <string_view>
#include <vector>



class Tokenizer
{
    std::string m_separator_tokens;
    size_t      m_single_sep_end;
    unsigned    m_tab_size = 4;

    constexpr static std::string_view m_discardable_separators = " \n\t\v";

public:
    struct Separators {
        std::string single, maybe_double ="";
    };

    using output_type = std::vector<Token>;

public:
    Tokenizer(Separators const&) noexcept;

    Tokenizer(
        std::string_view single_separator_tokens,
        std::string_view maybe_double_separator_tokens =""
    ) noexcept;

    void set_tab_size(unsigned);

    auto operator()(std::string_view) const -> output_type;

private:
    struct line_column_pair_t { size_t line, column; };

    auto _get_separator_length(std::string_view) const -> size_t;
    auto _handle_discardable(std::string_view)   const -> line_column_pair_t;

};
