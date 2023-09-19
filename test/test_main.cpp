#include <gtest/gtest.h>

//Basic example to verify that gtest is set up correctly
TEST(GTEST_TESTS, TEST_IF_GTEST_WORKS) {
EXPECT_STRNE("hello", "world");
EXPECT_EQ(7 * 6, 42);
}