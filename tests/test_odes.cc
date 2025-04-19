#include <gtest/gtest.h>

#include "tomathml.h"

// Test utilities headers.
#include "strings.h"

TEST(SimpleOde, ode1)
{
    std::string output = tomathml::process("ode(x, t) = 3{dimensionless};");
    EXPECT_EQ(expected_test_result_3, output);
}

TEST(SimpleOde, ode2)
{
    std::string output = tomathml::process("ode(y,t)=mu*(1{dimensionless}-sqr(x))*y-x;");
    EXPECT_EQ(expected_test_result_4, output);
}
