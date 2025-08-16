/// SIZE PROBLEMS!!!!

#include <gtest/gtest.h>

#include "scan.hpp"

using namespace stdx;

class StdxGroup : public testing::Test {
protected:
    

};

TEST_F(StdxGroup, ScanSimple) {
    // static_assert();
}

    // stdx::details::FixedString f{"q"};
    // constexpr stdx::details::FormatString<"Add {} to {%d}"> format;
    // ///                                                 
    // constexpr stdx::details::FixedString source("Add 125 to 3");

    // constexpr auto test = "qwert";
    // // constexpr stdx::details::FixedString t(test, test + 2);

    // using T = typename decltype(source)::Type;
    // static_assert(std::is_same_v<T, char>);

    // auto rt = format.getNumberPlaceholders();
    // decltype(format)::ErrorType error{"qwert"};

    // constexpr auto result = decltype(format)::placeholedrsNumber;
    // auto positions = decltype(format)::getPlaceholderPositions();
    // std::cout << "Positions: " << positions.size() << std::endl;
    // for (const auto &i : positions) {
    //     std::cout << i.first << ' ' << i.second << std::endl;
    // }
    // auto pair_1 = stdx::details::getCurrentSourceForParsing<0, format, source>();
    // auto pair_2 = stdx::details::getCurrentSourceForParsing<1, format, source>();
    // std::cout << "source placeholder " << pair_1.first << ' ' << pair_1.second << std::endl;
    // std::cout << "source placeholder " << pair_2.first << ' ' << pair_2.second << std::endl;

    // auto t = stdx::details::parseInput<0, format, source, int>();
    // std::cout << "t = " << t << std::endl;


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}