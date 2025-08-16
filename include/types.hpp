#include <cstddef>
#include <algorithm>
#include <type_traits>
#include <tuple>

#pragma once

namespace stdx {


template <typename CharStr, std::size_t N>
struct FixedString {
    using Type = CharStr;

public: 
    template <std::size_t M, typename = std::enable_if_t<M <= N>()>
    constexpr FixedString(const CharStr (&raw)[M]) noexcept : str{} {
        std::copy_n(raw, M, str);
    }

    // template <typename It, typename = std::enable_if_t<std::is_pointer_v<It>>>
    template <typename It>
    constexpr FixedString(It first, It last) noexcept : str{} {
        std::copy(first, last, str);
    }

    /// TODO actual size
    [[nodiscard]] constexpr std::size_t size() const noexcept {
        return N;
    }
    
    /// TODO fix return 
    constexpr decltype(auto) data() const noexcept {
        return &(str[0]);
    }

public:
    CharStr str[N];
};

template <typename CharStr, std::size_t S>
FixedString(const CharStr (&)[S]) -> FixedString<CharStr, S>;

template <FixedString ths, FixedString other> 
consteval bool compare()  noexcept {
    if (other.size() != ths.size()) {
        return false;
    }
    return std::equal(ths.data(), ths.data() + ths.size(), other.data());
}

///
///
///

namespace details {

    template <typename CharStr>
    struct ParseError : public FixedString<CharStr, 64> {
    };

}

///
///
///

template <typename... Ts>
struct ScanResult {
    
    constexpr ScanResult(Ts&&... ts) noexcept {
        values = std::make_tuple(std::forward<Ts>(ts)...);
    }
    std::tuple<Ts...> values;

    /// Todo Tuple has type???
    template <typename T>
    T value() const noexcept {
        return std::get<T>(values);
    }

    /// Return by value cause for there are only fundamental types
    template <std::size_t Ith, typename = std::enable_if_t<Ith <=sizeof... (Ts)>>
    auto value() const noexcept {
        return std::get<Ith>(values);
    }
};

} // namespace stdx::details
