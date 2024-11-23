#include "bytecode.hpp"
#include "gtest/gtest.h"

using namespace std::string_literals;

struct UtilRLETest : testing::Test {
protected:
    UtilRLETest()
    {
        rle.write_line_number(0, 1);
        rle.write_line_number(3, 1);
        rle.write_line_number(7, 2);
        rle.write_line_number(10, 3);
        rle.write_line_number(14, 5);
        rle.write_line_number(18, 10);
    }
    util::RLE rle;
};

TEST_F(UtilRLETest, ReadLineNumberInRLE)
{
    EXPECT_EQ(rle.read_line_number(1), 1);
    EXPECT_EQ(rle.read_line_number(2), 1);
    EXPECT_EQ(rle.read_line_number(3), 1);
    EXPECT_EQ(rle.read_line_number(4), 1);
    EXPECT_EQ(rle.read_line_number(5), 1);
    EXPECT_EQ(rle.read_line_number(6), 1);
    EXPECT_EQ(rle.read_line_number(7), 2);
    EXPECT_EQ(rle.read_line_number(8), 2);
    EXPECT_EQ(rle.read_line_number(9), 2);
    EXPECT_EQ(rle.read_line_number(10), 3);
    EXPECT_EQ(rle.read_line_number(11), 3);
    EXPECT_EQ(rle.read_line_number(12), 3);
    EXPECT_EQ(rle.read_line_number(13), 3);
    EXPECT_EQ(rle.read_line_number(14), 5);
    EXPECT_EQ(rle.read_line_number(15), 5);
    EXPECT_EQ(rle.read_line_number(16), 5);
    EXPECT_EQ(rle.read_line_number(17), 5);
    EXPECT_EQ(rle.read_line_number(18), 10);
    EXPECT_EQ(rle.read_line_number(120), 10);
}