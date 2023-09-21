//
// Created by Philip on 9/20/2023.
//
#include <gtest/gtest.h>
#include "../src/Rendering/EigenGraphicsUtils.h"
#include "../src/Units/AngleConversions.h"

TEST(UTILITY_TESTS, TEST_PROJECTION_MATRIX) {
    Eigen::Matrix4f projection_actual = EngiGraph::createPerspectiveProjection(EngiGraph::convert(EngiGraph::AngleDegrees<float>(90.0f)),4.0f/3.0f,0.1f,1000.0f);
    Eigen::Matrix4f projection_expected;
    projection_expected << //from GLM perspective function
            0.750000f , 0 , 0, 0,
            0 , 1.000000 , 0, 0,
            0 , 0 , -1.000200, -1.000000,
            0 , 0 , -0.200020, 0;
    for (int j = 0; j < 4; ++j) {
        for (int k = 0; k < 4; ++k) {
            ASSERT_FLOAT_EQ(projection_actual(j,k),projection_expected(j,k));
        }
    }
}