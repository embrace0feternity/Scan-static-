#include <algorithm>
#include <cstddef>
#include <tuple>
#include <type_traits>

#pragma once

namespace stdx {

template <typename CharStr, std::size_t N> struct FixedString {
  using Type = CharStr;

  template <std::size_t M, typename = std::enable_if_t<M <= N>()>
  constexpr FixedString(const CharStr (&raw)[M]) noexcept : str{} {
    std::copy_n(raw, M, str);
  }

  template <typename It, typename = std::enable_if_t<std::is_pointer_v<It>>>
  constexpr FixedString(It first, It last) noexcept : str{} {
    std::copy(first, last, str);
    str[N - 1] = '\0';
  }

  [[nodiscard]] constexpr std::size_t size() const noexcept { return N; }

  constexpr const CharStr *data() const noexcept { return str; }

  CharStr str[N];
};

template <typename CharStr, std::size_t S>
FixedString(const CharStr (&)[S]) -> FixedString<CharStr, S>;

template <FixedString ths, FixedString other>
consteval bool isEqual() noexcept {
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
struct ParseError : public FixedString<CharStr, 64> {};

} // namespace details

///
///
///

template <typename... Ts> struct ScanResult {
  constexpr ScanResult(Ts &&...ts) noexcept
      : values(std::make_tuple(std::forward<Ts>(ts)...)) {}

  /// Return by value because there are only fundamental types
  template <std::size_t Ith, typename = std::enable_if_t<Ith <= sizeof...(Ts)>>
  constexpr std::tuple_element_t<Ith, std::tuple<Ts...>>
  value() const noexcept {
    return std::get<Ith>(values);
  }

  std::tuple<Ts...> values;
};

} // namespace stdx
