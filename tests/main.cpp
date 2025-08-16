/// SIZE PROBLEMS!!!!

#include <gtest/gtest.h>

#include "scan.hpp"
#include <iostream>

using namespace stdx;

class StdxGroup : public testing::Test {};

TEST_F(StdxGroup, CheckPatterns) {
    /// Existed specifiers
    constexpr FixedString empty{""};
    constexpr FixedString d{"%d"};
    constexpr FixedString u{"%u"};
    constexpr FixedString s{"%s"};
    /// Non-exist
    constexpr FixedString n_1{"%non-exist pattern"};
    constexpr FixedString n_2{"%D"};
    constexpr FixedString n_3{"%AmIExist?"};

    static_assert(
        details::checkPattern<empty>() && 
        details::checkPattern<d>() &&
        details::checkPattern<u>() && 
        details::checkPattern<s>() &&
        !details::checkPattern<n_1>() &&
        !details::checkPattern<n_2>() &&
        !details::checkPattern<n_3>()
    );
    ASSERT_TRUE(true);
}


TEST_F(StdxGroup, CheckParseValue) {
    auto check = []<FixedString pattern, typename T>()consteval {
        return std::is_same_v<typename details::ParseValue<pattern, T>::Type, T>;
    };

    /// int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t, std::string_view
    /// TODO ADD CV
    static_assert(true &&
        check.operator()<""_fx, int>() &&
        check.operator()<""_fx, int8_t>() &&
        check.operator()<""_fx, int16_t>() &&
        check.operator()<"%d"_fx, int32_t>() &&
        check.operator()<"%d"_fx, int64_t>() &&
        ///
        check.operator()<"%u"_fx, uint8_t>() &&
        check.operator()<"%u"_fx, uint16_t>() &&
        check.operator()<"%u"_fx, uint32_t>() &&
        check.operator()<"%u"_fx, uint64_t>() &&
        ///
        check.operator()<"%s"_fx, std::string_view>() 
    );
    ASSERT_TRUE(true);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}