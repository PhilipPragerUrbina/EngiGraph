//
// Created by Philip on 9/20/2023.
//
#include <gtest/gtest.h>
#include "../src/Rendering/EigenGraphicsUtils.h"
#include "../src/Math/AngleConversions.h"

//We test against GLM as a reference implementation here since these utilities are supposed to mimic GLM.

TEST(UTILITY_TESTS, TEST_PROJECTION_MATRIX) {
    Eigen::Matrix4f projection_actual = EngiGraph::createPerspectiveProjection(EngiGraph::convert(EngiGraph::AngleDegrees<float>(90.0f)),4.0f/3.0f,0.1f,1000.0f);
    Eigen::Matrix4f projection_expected;
    projection_expected << //from GLM perspective function
            0.750000f , 0 , 0, 0,
            0 , 1.000000 , 0, 0,
            0 , 0 , -1.000200, -1.000000,
            0 , 0 , -0.200020, 0;
    projection_expected.transposeInPlace(); //GLM is column major, but was printed as row major.
    for (int j = 0; j < 4; ++j) {
        for (int k = 0; k < 4; ++k) {
            ASSERT_FLOAT_EQ(projection_actual(j,k),projection_expected(j,k));
        }
    }
}

TEST(UTILITY_TESTS, TEST_VIEW_MATRIX) {
    Eigen::Matrix4f view_actual = EngiGraph::createLookAtView(Eigen::Vector3f{10.0f,10.0f,10.0f}, Eigen::Vector3f{5.0f,5.0f,5.0f}, Eigen::Vector3f{0.0f,1.0f,0.0f});
    Eigen::Matrix4f view_expected;
    view_expected << //from GLM lookAt function
            0.707107 , -0.408248 , 0.577350 , 0.000000 ,
            0.000000 , 0.816496 , 0.577350 ,0.000000 ,
            -0.707107, -0.408248, 0.577350, 0.000000 ,
            -0.000000, -0.000000, -17.320505, 1.000000;
    view_expected.transposeInPlace();//GLM is column major, but was printed as row major.
    for (int j = 0; j < 4; ++j) { //These values are not as clean as the last test, so a delta is used.
        for (int k = 0; k < 4; ++k) {
            ASSERT_NEAR(view_actual(j,k),view_expected(j,k), 0.001f);
        }
    }
}