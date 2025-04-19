#include <gtest/gtest.h>

#include "tomathml.h"

// Test utilities headers.
#include "strings.h"

TEST(SimpleEqn, AeqB)
{
    std::string output = tomathml::process("a = b;");
    EXPECT_EQ(expected_test_result_1, output);
}

TEST(SimpleEqn, AeqBandCeqD)
{
    std::string output = tomathml::process("a = b;\nc = d;");
    EXPECT_EQ(expected_test_result_2, output);
}

TEST(SimpleEqn, AeqBandCeqDplusE)
{
    std::string output = tomathml::process("a = b;c = d + e;");
    EXPECT_EQ(expected_test_result_5, output);
}

TEST(SimpleEqn, AeqBplusNumber)
{
    std::string output = tomathml::process("a = b - 5{kilogram};");
    EXPECT_EQ(expected_test_result_6, output);
}

TEST(SimpleEqn, AeqBplusNumberCellMLOff)
{
    std::string output = tomathml::process("a = b + 3;", false);
    EXPECT_EQ(expected_test_result_7, output);
}
