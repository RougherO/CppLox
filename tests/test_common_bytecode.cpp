#include "bytecode.hpp"
#include "gtest/gtest.h"

struct ByteCodeTest : testing::Test {
protected:
    ByteCodeTest()
    {
        bcode.write_byte(0b101, 1);
        bcode.write_byte(0b100, 1);
        bcode.write_byte(0b1000, 2);
        bcode.write_byte(0b1001, 2);
        bcode.write_byte(0b1100, 2);
    }
    ByteCode bcode;
};

TEST_F(ByteCodeTest, ReadLineNumberAtByteOffsetOnSameLine)
{
    EXPECT_EQ(bcode.read_line_number(0), 1);
    EXPECT_EQ(bcode.read_line_number(1), 1);
    EXPECT_EQ(bcode.read_line_number(2), 2);
    EXPECT_EQ(bcode.read_line_number(3), 2);
    EXPECT_EQ(bcode.read_line_number(4), 2);
}