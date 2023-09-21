//
// Created by Philip on 9/20/2023.
//
#include "gtest/gtest.h"
#include "../src/Units/AngleConversions.h"
#include "../src/Units/Constants.h"
TEST(CONVERSION_TESTS, TEST_ANGLE_CONVERSIONS) {
    //Zero float
    ASSERT_FLOAT_EQ(EngiGraph::convert(EngiGraph::AngleRadians(0.0f)).value,0.0f);
    ASSERT_FLOAT_EQ(EngiGraph::convert(EngiGraph::AngleDegrees(0.0f)).value,0.0f);

    //PI float
    ASSERT_FLOAT_EQ(EngiGraph::convert(EngiGraph::AngleRadians((float)EngiGraph::CONSTANT_PI)).value,180.0f);
    ASSERT_FLOAT_EQ(EngiGraph::convert(EngiGraph::AngleDegrees(180.0f)).value,(float)EngiGraph::CONSTANT_PI);

    //PI double
    ASSERT_FLOAT_EQ(EngiGraph::convert(EngiGraph::AngleRadians((double)EngiGraph::CONSTANT_PI)).value,180.0);
    ASSERT_FLOAT_EQ(EngiGraph::convert(EngiGraph::AngleDegrees(180.0)).value,(double)EngiGraph::CONSTANT_PI);
}