#pragma once

#include "parse.hpp"
#include "format_string.hpp"
#include "types.hpp"

namespace stdx {

namespace details {

    template <FormatString fmt, FixedString source, typename... Ts, std::size_t... sequence>
    constexpr ScanResult<Ts...> goThrough(std::integer_sequence<std::size_t, sequence...>) noexcept {
        return { parseInput<sequence, fmt, source, Ts>()... };
    }

}

template <FormatString fmt, FixedString source, typename... Ts>
consteval ScanResult<Ts...> scan() noexcept { 
    using Range = std::make_integer_sequence<std::size_t, sizeof...(Ts)>;
    return details::goThrough<fmt, source, Ts...>(Range{});
}

} // namespace stdx