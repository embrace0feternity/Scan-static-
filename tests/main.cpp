/// SIZE PROBLEMS!!!!

#include <gtest/gtest.h>

#include "scan.hpp"
#include <type_traits>

using namespace stdx;

class StdxGroup : public testing::Test {};

///
///
///

TEST_F(StdxGroup, Patterns) {
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

///
///
///

TEST_F(StdxGroup, ParseValue) {
    auto checkValue = []<FixedString pattern, FixedString source, typename T>()consteval {
        return details::ParseValue<pattern, T>::template convert<source>().has_value();
    };

    auto checkType = []<FixedString pattern, FixedString source, typename T>()consteval {
        return std::is_same_v<std::remove_reference_t<
            decltype(details::ParseValue<pattern, T>::template convert<source>().value())>, 
            T>;
    };

    /// int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t, std::string_view
    using S = std::string_view;
    using C_S = const std::string_view;
    using CV_S = const volatile std::string_view;

    static_assert(true &&
        checkValue.operator()<""_fx, "-6"_fx, int>() &&
        checkValue.operator()<""_fx, "-6"_fx, int8_t>() &&
        checkValue.operator()<""_fx, "-6"_fx, int16_t>() &&
        checkValue.operator()<"%d"_fx, "-6"_fx, int32_t>() &&
        checkValue.operator()<"%d"_fx, "-6"_fx, int64_t>() &&
        checkValue.operator()<"%u"_fx, "12"_fx, uint8_t>() &&
        checkValue.operator()<"%u"_fx, "12"_fx, uint16_t>() &&
        checkValue.operator()<"%u"_fx, "12"_fx, uint32_t>() &&
        checkValue.operator()<"%u"_fx, "12"_fx, uint64_t>() &&
        checkValue.operator()<"%s"_fx, "12"_fx, std::string_view>() &&
        /// Add CV
        checkValue.operator()<""_fx, "12"_fx, const int>() &&
        checkValue.operator()<"%d"_fx, "12"_fx, const volatile std::int8_t>() &&
        checkValue.operator()<"%u"_fx, "12"_fx, const std::uint32_t>() &&
        checkValue.operator()<"%u"_fx, "12"_fx, const volatile std::uint8_t>() &&
        checkValue.operator()<"%s"_fx, "12"_fx, S>() &&
        checkValue.operator()<"%s"_fx, "12"_fx, C_S>() &&
        checkValue.operator()<"%s"_fx, "12"_fx, CV_S>() &&
        /// Check types
        checkType.operator()<""_fx, "12"_fx, const int>() &&
        checkType.operator()<""_fx, "12"_fx, const volatile std::int8_t>() &&
        checkType.operator()<"%u"_fx, "12"_fx, const std::uint32_t>() &&
        checkType.operator()<"%u"_fx, "12"_fx, const volatile std::uint8_t>() &&
        checkType.operator()<"%s"_fx, "12"_fx, S>() &&
        checkType.operator()<"%s"_fx, "12"_fx, C_S>() &&
        checkType.operator()<"%s"_fx, "12"_fx, CV_S>() &&
        /// 
        !checkValue.operator()<"%s"_fx, "12"_fx, int>() &&
        !checkValue.operator()<"%s"_fx, "12"_fx, std::uint64_t>() &&
        !checkValue.operator()<"%s"_fx, "12"_fx, std::string>() &&
        !checkValue.operator()<"%u"_fx, "-12"_fx, std::uint16_t>() &&
        !checkValue.operator()<"%s"_fx, "12"_fx, const std::int64_t>() &&
        !checkValue.operator()<""_fx, "12"_fx, C_S>() &&
        !checkValue.operator()<"%d"_fx, "12"_fx, CV_S>() &&
        !checkValue.operator()<"%s"_fx, "12"_fx, std::uint8_t>() &&
        /// Check ptr and ref
        !checkValue.operator()<""_fx, "12"_fx, int &>() &&
        !checkValue.operator()<"%u"_fx, "12"_fx, const std::uint32_t *>() &&
        !checkValue.operator()<"%s"_fx, "12"_fx, const std::string_view &>()
    );
    auto value_1 = details::ParseValue<"%u"_fx, std::uint32_t>::template convert<"6122"_fx>().value();
    auto value_2 = details::ParseValue<"%d"_fx, const volatile std::int16_t>::template convert<"6122"_fx>().value();
    auto value_3 = details::ParseValue<""_fx, std::int32_t>::template convert<"-6122"_fx>().value();
    auto value_4 = details::ParseValue<"%s"_fx, C_S>::template convert<"-6122"_fx>().value();
    ASSERT_EQ(6122, value_1);
    ASSERT_EQ(6122, value_2);
    ASSERT_EQ(-6122, value_3);
    ASSERT_EQ("-6122", value_4);
}

///
///
///

TEST_F(StdxGroup, ParseInput)
{
    {
        auto value = details::parseInput<0, "{}"_frm, "12"_fx, std::int8_t>();
        static_assert(std::is_same_v<decltype(value), std::int8_t>);
        ASSERT_EQ(12, value);
    }
    {
        auto value = details::parseInput<0, "{%s}"_frm, "12"_fx, const std::string_view>();
        static_assert(std::is_same_v<decltype(value), std::string_view>);
        ASSERT_EQ("12", value);
    }
    {
        auto value = details::parseInput<0, "Add {%u} to 3"_frm, "Add 2 to 3"_fx, std::uint8_t>();
        static_assert(std::is_same_v<decltype(value), std::uint8_t>);
        ASSERT_EQ(2, value);
    }
}

///
///
///

TEST_F(StdxGroup, Errors) {
    /// Broken format string
    // constexpr auto frm = "}{"_frm;
    // constexpr auto frm = "{%s"_frm;
    // constexpr auto frm = "{%}"_frm;
    // constexpr auto frm = "{qwer}"_frm;

    /// Incorrect type/specifier
    constexpr auto frm = "{%s} {%u} {%s} {} {%s} {%s} {%d}."_frm;
    constexpr auto source = "Divide 8236536 into 4118268 and add 3"_fx;
    // auto result = scan<frm, source, std::uint64_t>();
    // auto result = scan<frm, source, std::string_view*>();
    // auto result = scan<frm, source, std::string_view, std::uint32_t>();

    /// Fail to convert
    constexpr auto errorSource = "Divide -8236536 into [4118268] and add 3_fx"_fx;
    // auto result = scan<frm, errorSource, std::string_view, std::uint32_t>();
    // auto result = scan<frm, errorSource, std::string_view, std::uint32_t, std::string_view, int>();

    /// Overload type
    constexpr auto overloadSource = "Divide -823 into 411 and add 3_fx"_fx;
    // auto result = scan<frm, overloadSource, std::string_view, std::uint8_t>();
    // auto result = scan<frm, overloadSource, std::string_view, std::uint32_t, std::string_view, std::int8_t>();
}

///
///
///

TEST_F(StdxGroup, Scan) {
    {
        constexpr auto frm = "I want to sum {} and {%u} numbers."_frm;
        constexpr auto source = "I want to sum 12 and 3 numbers."_fx;
        auto result = scan<frm, source, std::int8_t, std::uint64_t>();
        auto i8 = result.value<0>();
        auto u64 = result.value<1>();
        static_assert(std::is_same_v<decltype(i8), std::int8_t>);
        static_assert(std::is_same_v<decltype(u64), std::uint64_t>);
        ASSERT_EQ(12, i8);
        ASSERT_EQ(3, u64);
    }
    {
        constexpr auto frm = "{%s} {} {%s} {%d} {%s} {%s} {%u}."_frm;
        constexpr auto source = "Divide 8236536 into 4118268 and add 3"_fx;
        
        auto result = scan<frm, source, 
                    std::string_view,
                    std::int32_t,
                    const std::string_view,
                    int,
                    std::string_view,
                    const std::string_view,
                    const std::uint64_t
                    >();

        auto v_1 = result.value<0>();
        auto v_2 = result.value<1>();
        auto v_3 = result.value<2>();
        auto v_4 = result.value<3>();
        auto v_5 = result.value<4>();
        auto v_6 = result.value<5>();
        auto v_7 = result.value<6>();
        static_assert(std::is_same_v<decltype(v_1), std::string_view>);
        static_assert(std::is_same_v<decltype(v_2), int32_t>);
        static_assert(std::is_same_v<decltype(v_3), std::string_view>);
        static_assert(std::is_same_v<decltype(v_4), int>);
        static_assert(std::is_same_v<decltype(v_5), std::string_view>);
        static_assert(std::is_same_v<decltype(v_6), std::string_view>);
        static_assert(std::is_same_v<decltype(v_7), std::uint64_t>);
        static_assert(std::is_same_v<std::tuple_element_t<0, decltype(result.values)>, std::string_view>);
        static_assert(std::is_same_v<std::tuple_element_t<1, decltype(result.values)>, std::int32_t>);
        static_assert(std::is_same_v<std::tuple_element_t<2, decltype(result.values)>, const std::string_view>);
        static_assert(std::is_same_v<std::tuple_element_t<3, decltype(result.values)>, int>);
        static_assert(std::is_same_v<std::tuple_element_t<4, decltype(result.values)>, std::string_view>);
        static_assert(std::is_same_v<std::tuple_element_t<5, decltype(result.values)>, const std::string_view>);
        static_assert(std::is_same_v<std::tuple_element_t<6, decltype(result.values)>, const std::uint64_t>);
        ASSERT_EQ("Divide", v_1);
        ASSERT_EQ(8236536, v_2);
        ASSERT_EQ("into", v_3);
        ASSERT_EQ(4118268, v_4);
        ASSERT_EQ("and", v_5);
        ASSERT_EQ("add", v_6);
        ASSERT_EQ(3, v_7);
    }
}

///
///
///

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}