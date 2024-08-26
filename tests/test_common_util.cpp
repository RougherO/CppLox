#include "util.hpp"
#include "gtest/gtest.h"

using namespace std::string_literals;

TEST(UtilToStringTest, IntToString)
{
    EXPECT_EQ(Util::to_string(Int { 0 }), "0");
    EXPECT_EQ(Util::to_string(Int { 1 }), "1");
    EXPECT_EQ(Util::to_string(Int { 2 }), "2");
    EXPECT_EQ(Util::to_string(Int { 3 }), "3");
}

TEST(UtilToStringTest, FloatToStringNoDecimal)
{
    EXPECT_EQ(Util::to_string(Float { 0 }), "0.0"s);
    EXPECT_EQ(Util::to_string(Float { 1 }), "1.0"s);
    EXPECT_EQ(Util::to_string(Float { 2 }), "2.0"s);
    EXPECT_EQ(Util::to_string(Float { 3 }), "3.0"s);
}

TEST(UtilToStringTest, FloatToStringWithDecimal)
{
    EXPECT_EQ(Util::to_string(Float { 0.5 }), "0.5"s);
    EXPECT_EQ(Util::to_string(Float { 1.5 }), "1.5"s);
    EXPECT_EQ(Util::to_string(Float { 2.5 }), "2.5"s);
    EXPECT_EQ(Util::to_string(Float { 3.5 }), "3.5"s);
}