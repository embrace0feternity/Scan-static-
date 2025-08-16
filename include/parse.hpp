#pragma once

#include <charconv>
#include <concepts>

#include "format_string.hpp"
#include "types.hpp"

namespace stdx::details {

/// int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t, std::string_view

template <FixedString specifier>
consteval bool checkPattern() {
    constexpr std::string_view patterns[] = {"", "%d", "%u", "%s"};
    for (const auto& pattern : patterns) {
        if (std::equal(specifier.data(), specifier.data() + specifier.size(), pattern.begin())) {
            return true;
        }
    }
    return false;
}

///
///
///

template <FixedString specifier, typename SourceType, typename = void> 
struct ParseValue {
    using Type = void;
    static_assert(false, "Unsupported specifier");
};

template <FixedString specifier, typename SourceType> 
struct ParseValue<specifier, SourceType> {

    using Type = SourceType;

    static_assert(((compare<specifier, ""_fx>() ||
                    compare<specifier, "%d"_fx>()) &&
                    std::is_signed_v<SourceType>) 
                    ||
                    (compare<specifier, "%u"_fx>() &&
                    std::is_unsigned_v<SourceType>),
                    "Type/specifier mismatch");  

    template <FixedString strType> static constexpr SourceType convert() noexcept {
      SourceType value;
      std::from_chars(strType.data(), strType.data() + strType.size(), value);
      // std::cout << "raw string is " << strType.data() << "  value is " << value
      // << std::endl;
      return value;
    }
};

template <> 
struct ParseValue<"%s"_fx, std::string_view> {
    using Type = std::string_view;
    static constexpr bool error = true;

    template <FixedString strType> static constexpr std::string_view convert() noexcept {
      return strType.data();
    }
};

///
///
///

template <int I, FormatString fmt, FixedString source>
consteval auto getCurrentSourceForParsing() {
  static_assert(I >= 0 && I < fmt.placeholedrsNumber,
                "Invalid placeholder index");

  constexpr auto to_sv = [](const auto &fs) {
    return std::string_view(fs.data(), fs.size() - 1);
  };

  constexpr auto fmt_sv = to_sv(fmt.fmt);
  constexpr auto src_sv = to_sv(source);
  constexpr auto &positions = fmt.placeholedrsPositions;

  // Получаем границы текущего плейсхолдера в формате
  constexpr auto pos_i = positions[I];
  constexpr size_t fmt_start = pos_i.first, fmt_end = pos_i.second;

  // Находим начало в исходной строке
  constexpr auto src_start = [&] {
    if constexpr (I == 0) {
      return fmt_start;
    } else {
      // Находим конец предыдущего плейсхолдера в исходной строке
      constexpr auto prev_bounds =
          getCurrentSourceForParsing<I - 1, fmt, source>();
      const auto prev_end = prev_bounds.second;

      // Получаем разделитель между текущим и предыдущим плейсхолдерами
      constexpr auto prev_fmt_end = positions[I - 1].second;
      constexpr auto sep =
          fmt_sv.substr(prev_fmt_end + 1, fmt_start - (prev_fmt_end + 1));

      // Ищем разделитель после предыдущего значения
      auto pos = src_sv.find(sep, prev_end);
      return pos != std::string_view::npos ? pos + sep.size() : src_sv.size();
    }
  }();

  // Находим конец в исходной строке
  constexpr auto src_end = [&] {
    // Получаем разделитель после текущего плейсхолдера
    if constexpr (fmt_end == (fmt_sv.size() - 1)) {
      return src_sv.size();
    }
    constexpr auto sep =
        fmt_sv.substr(fmt_end + 1, (I < fmt.placeholedrsNumber - 1)
                                       ? positions[I + 1].first - (fmt_end + 1)
                                       : fmt_sv.size() - (fmt_end + 1));
    // Ищем разделитель после текущего значения
    constexpr auto pos = src_sv.find(sep, src_start);
    return pos != std::string_view::npos ? pos : src_sv.size();
  }();
  return std::pair{src_start, src_end};
}

///
///
///

template <std::size_t I, FormatString fmt, FixedString source, typename T>
consteval T parseInput() {

  constexpr auto ith = getCurrentSourceForParsing<I, fmt, source>();
  constexpr FixedString<typename decltype(source)::Type,
                        std::distance(&source.data()[ith.first],
                                      &source.data()[ith.second])>
      subString(&source.data()[ith.first], &source.data()[ith.second]);
  // return ParseValue<T>::template convert<subString>();
  return T{};
}

} // namespace stdx::details
