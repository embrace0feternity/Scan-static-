#pragma once

#include "types.hpp"
#include <array>
#include <expected>
#include <string_view>


namespace stdx {

template <FixedString formatString> class FormatString {
public:
  using ErrorType = details::ParseError<typename decltype(formatString)::Type>;
  using Rt = std::expected<std::size_t, ErrorType>;

private:
  static constexpr Rt getNumberPlaceholders();

  static constexpr auto getPlaceholderPositions();

public:
  static constexpr FixedString fmt = formatString;

  static constexpr std::size_t placeholedrsNumber = []() {
    constexpr auto n = getNumberPlaceholders();
    if constexpr (!n) {
      static_assert(false, n.error());
      return 0;
    } else {
      return n.value();
    }
  }();

  static constexpr auto placeholedrsPositions = getPlaceholderPositions();
};

template <FixedString formatString>
constexpr auto FormatString<formatString>::getPlaceholderPositions() {
  std::array<std::pair<std::size_t, std::size_t>, placeholedrsNumber>
      placeholdersPositions;
  auto ithPlaceholder = placeholdersPositions.begin();
  std::size_t openPos = std::string_view::npos;
  auto formatStringRaw = formatString.data();

  for (std::size_t i = 0; i < formatString.size(); ++i) {
    if (formatStringRaw[i] != '{') {
      continue;
    }
    openPos = i;
    for (std::size_t closePos = openPos; closePos < formatString.size();
         ++closePos) {
      if (formatStringRaw[closePos] == '}') {
        *ithPlaceholder++ = std::make_pair(openPos, closePos);
        i = closePos;
        break;
      }
    }
  }

  return placeholdersPositions;
}

template <FixedString formatString>
constexpr typename FormatString<formatString>::Rt
FormatString<formatString>::getNumberPlaceholders() {
  constexpr size_t N = formatString.size();
  if (N == 1) {
    return std::unexpected(ErrorType{"Format string is empty"});
  }
  size_t placeholder_count = 0;
  size_t pos = 0;
  const size_t size = N - 1; // -1 для игнорирования нуль-терминатора
  while (pos < size) {
    // Пропускаем все символы до '{'
    if (formatString.str[pos] != '{') {
      ++pos;
      continue;
    }

    // Проверяем незакрытый плейсхолдер
    if (pos + 1 >= size) {
      return std::unexpected(ErrorType{"Unclosed last placeholder"});
    }

    // Начало плейсхолдера
    ++placeholder_count;
    ++pos;

    // Проверка спецификатора формата
    if (formatString.str[pos] == '%') {
      ++pos;
      if (pos >= size) {
        return std::unexpected(ErrorType{"Unclosed last placeholder"});
      }

      // Проверяем допустимые спецификаторы
      const char spec = formatString.str[pos];
      constexpr char valid_specs[] = {'d', 'u', 'f', 's'};
      bool valid = false;

      for (const char s : valid_specs) {
        if (spec == s) {
          valid = true;
          break;
        }
      }

      if (!valid) {
        return std::unexpected(ErrorType{"Invalid specifier."});
      }
      ++pos;
    }

    // Проверяем закрывающую скобку
    if (pos >= size || formatString.str[pos] != '}') {
      return std::unexpected(
          ErrorType{"\'}\' hasn't been found in appropriate place"});
    }
    ++pos;
  }
  return placeholder_count;
}

template <FixedString frm> constexpr auto operator""_frm() {
  return FormatString<frm>{};
}

template <FixedString source> constexpr auto operator""_fx() {
  return FixedString{source};
}

} // namespace stdx
