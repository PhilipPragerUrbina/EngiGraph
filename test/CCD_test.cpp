//
// Created by Philip on 10/8/2023.
//
#include "gtest/gtest.h"
#include "../src/Physics/Collisions/LinearPointCcd.h"
#include "../src/Physics/Collisions/LinearPointCcd.cpp"

TEST(INTERSECTION_TESTS, TEST_WATERTIGHT_RAY_TRIANGLE){
    //Test a prerequisite ray functions.
    {
        //This function should return the largest magnitude dimension in kz.
        Eigen::Vector3d ray_direction{2,14,16};
        auto axis = EngiGraph::calculateRayDimensions(ray_direction);
        Eigen::Matrix<uint8_t,3,1> expected{0,1,2}; //Axis 2(z) has the most.
        ASSERT_EQ(axis,expected);
        //Magnitude so sign does not matter
        ray_direction = {2,14,-16};
        axis = EngiGraph::calculateRayDimensions(ray_direction);
        expected={0,1,2};
        ASSERT_EQ(axis,expected);

        ray_direction = {0,14,0};
        axis = EngiGraph::calculateRayDimensions(ray_direction);
        expected={2,0,1};
        ASSERT_EQ(axis,expected);
    }
    {
        Eigen::Vector3d ray_direction{1.0f,0.0f,0.0f};
        ray_direction.normalize();
        auto k = EngiGraph::calculateRayDimensions(ray_direction);
        Eigen::Vector3d expected = {0.0f,0.0f,1.0f};
        ASSERT_EQ(EngiGraph::calculateRayShearConstraints(k,ray_direction),expected);
    }
    //These results are verified using Desmos 3D graphing!
    //ray in front of triangle facing away
    Eigen::Vector3d direction = {1.0,0.0,0.0};
    Eigen::Vector3d origin = {1.0,0.0,0.0};
    direction.normalize();
    auto k = EngiGraph::calculateRayDimensions(direction);
    auto s = EngiGraph::calculateRayShearConstraints(k,direction);
    Eigen::Vector3d a = {0.0,-1.0,-1.0};
    Eigen::Vector3d b = {0.0,1.0,-1.0};
    Eigen::Vector3d c = {0.0,1.0,1.0};
    Eigen::Vector4d hit_info{};
    ASSERT_FALSE(EngiGraph::rayTriangleIntersection(a,b,c,origin,hit_info,k,s));
    //ray in front facing towards triangle
    direction = {-1.0,0.0,0.0};
    origin = {1.0,0.5,-0.5};
    direction.normalize();
    k = EngiGraph::calculateRayDimensions(direction);
    s = EngiGraph::calculateRayShearConstraints(k,direction);
    a = {0.0,-1.0,-1.0};
    b = {0.0,1.0,-1.0};
    c = {0.0,1.0,1.0};
    ASSERT_TRUE(EngiGraph::rayTriangleIntersection(a,b,c,origin,hit_info,k,s));
    ASSERT_DOUBLE_EQ(hit_info.w(), 1.0); //distance
    //parallel line offset
    direction = {0.0,1.0,0.0};
    origin = {1.0,0.0,0.0};
    direction.normalize();
    k = EngiGraph::calculateRayDimensions(direction);
    s = EngiGraph::calculateRayShearConstraints(k,direction);
    a = {0.0,-1.0,-1.0};
    b = {0.0,1.0,-1.0};
    c = {0.0,1.0,1.0};
    ASSERT_FALSE(EngiGraph::rayTriangleIntersection(a,b,c,origin,hit_info,k,s));
    //parallel line on
    direction = {0.0,1.0,0.0};
    origin = {0.0,0.0,0.0};
    direction.normalize();
    k = EngiGraph::calculateRayDimensions(direction);
    s = EngiGraph::calculateRayShearConstraints(k,direction);
    a = {0.0,-1.0,-1.0};
    b = {0.0,1.0,-1.0};
    c = {0.0,1.0,1.0};
    ASSERT_FALSE(EngiGraph::rayTriangleIntersection(a,b,c,origin,hit_info,k,s));
    //ray above triangle
    direction = {-1.0,0.0,0.0};
    origin = {1.0,2.0,-0.5};
    direction.normalize();
    k = EngiGraph::calculateRayDimensions(direction);
    s = EngiGraph::calculateRayShearConstraints(k,direction);
    a = {0.0,-1.0,-1.0};
    b = {0.0,1.0,-1.0};
    c = {0.0,1.0,1.0};
    ASSERT_FALSE(EngiGraph::rayTriangleIntersection(a,b,c,origin,hit_info,k,s));
    //Glancing hit
    direction = {-1.0,0.9,0.9};
    origin = {0.5,0.5,-0.5};
    direction.normalize();
    k = EngiGraph::calculateRayDimensions(direction);
    s = EngiGraph::calculateRayShearConstraints(k,direction);
    a = {0.0,-1.0,-1.0};
    b = {0.0,1.0,-1.0};
    c = {0.0,1.0,1.0};
    ASSERT_TRUE(EngiGraph::rayTriangleIntersection(a,b,c,origin,hit_info,k,s));
    //Vertex hit
    direction = {-1.0,0.0,0.0};
    origin = {1.0,1.0,1.0};
    direction.normalize();
    k = EngiGraph::calculateRayDimensions(direction);
    s = EngiGraph::calculateRayShearConstraints(k,direction);
    a = {0.0,-1.0,-1.0};
    b = {0.0,1.0,-1.0};
    c = {0.0,1.0,1.0};
    ASSERT_TRUE(EngiGraph::rayTriangleIntersection(a,b,c,origin,hit_info,k,s));
    //Check barycentric coordinates
    ASSERT_DOUBLE_EQ(hit_info.x(),0.0); //U
    ASSERT_DOUBLE_EQ(hit_info.y(),0.0); //V
    ASSERT_DOUBLE_EQ(hit_info.z(),1.0); //W
}

TEST(INTERSECTION_TESTS, TEST_RAY_PATCH){
    //Test a prerequisite functions.
    {
        Eigen::Vector3d a = {0.2,0.3,0.1};
        Eigen::Vector3d b = {-0.3,1.0,3.0};
        double t = 0.7;
        auto result = EngiGraph::lerp(a,b,t);
        ASSERT_DOUBLE_EQ(result.x(), -0.15);
        ASSERT_DOUBLE_EQ(result.y(), 0.79);
        ASSERT_DOUBLE_EQ(result.z(), 2.13);
    }

    //These results are verified using Desmos 3D graphing!
    //See this if possible: https://www.desmos.com/3d/76fff98684

    //Facing away from planar quad
    Eigen::Vector3d direction = {1.0,0.0,0.0};
    Eigen::Vector3d origin = {1.0,0.0,0.0};
    direction.normalize();
    Eigen::Vector3d a = {0.0,-1.0,-1.0};
    Eigen::Vector3d b = {0.0,-1.0,1.0};
    Eigen::Vector3d c = {0.0,1.0,-1.0};
    Eigen::Vector3d d = {0.0,1.0,1.0};
    Eigen::Vector3d hit_info{};
    ASSERT_FALSE(EngiGraph::rayQuadPatchIntersection(a,b,c,d,origin,direction,hit_info,100.0));
    //Facing towards planar quad
    direction = {-1.0,0.0,0.0};
    origin = {1.0,0.0,0.0};
    direction.normalize();
    a = {0.0,-1.0,-1.0};
    b = {0.0,-1.0,1.0};
    c = {0.0,1.0,-1.0};
    d = {0.0,1.0,1.0};
    ASSERT_TRUE(EngiGraph::rayQuadPatchIntersection(a,b,c,d,origin,direction,hit_info,100.0));
    ASSERT_DOUBLE_EQ(hit_info.z(),1.0); //distance
    ASSERT_DOUBLE_EQ(hit_info.x(),0.5); //u hit at center
    ASSERT_DOUBLE_EQ(hit_info.y(),0.5); //v
    //Facing towards planar quad other side
    direction = {1.0,0.0,0.0};
    origin = {-1.0,0.0,0.0};
    direction.normalize();
    a = {0.0,-1.0,-1.0};
    b = {0.0,-1.0,1.0};
    c = {0.0,1.0,-1.0};
    d = {0.0,1.0,1.0};
    ASSERT_TRUE(EngiGraph::rayQuadPatchIntersection(a,b,c,d,origin,direction,hit_info,100.0));
    ASSERT_DOUBLE_EQ(hit_info.z(),1.0); //distance
    ASSERT_DOUBLE_EQ(hit_info.x(),0.5); //u hit at center
    ASSERT_DOUBLE_EQ(hit_info.y(),0.5); //v
    //Facing towards but missing planar quad
    direction = {-1.0,0.0,0.0};
    origin = {1.0,2.0,0.0};
    direction.normalize();
    a = {0.0,-1.0,-1.0};
    b = {0.0,-1.0,1.0};
    c = {0.0,1.0,-1.0};
    d = {0.0,1.0,1.0};
    ASSERT_FALSE(EngiGraph::rayQuadPatchIntersection(a,b,c,d,origin,direction,hit_info,100.0));
    //Facing towards planar quad but outside of distance max
    direction = {-1.0,0.0,0.0};
    origin = {1.0,0.0,0.0};
    direction.normalize();
    a = {0.0,-1.0,-1.0};
    b = {0.0,-1.0,1.0};
    c = {0.0,1.0,-1.0};
    d = {0.0,1.0,1.0};
    ASSERT_FALSE(EngiGraph::rayQuadPatchIntersection(a,b,c,d,origin,direction,hit_info,0.5));
    //Facing towards a non-planar quad
    direction = {-1.0,0.0,0.0};
    origin = {1.0,0.0,0.0};
    direction.normalize();
    a = {-1.0,-1.0,-1.0};
    b = {0.0,-1.0,1.0};
    c = {0.0,2.0,-1.0};
    d = {0.0,1.0,1.0};
    ASSERT_TRUE(EngiGraph::rayQuadPatchIntersection(a,b,c,d,origin,direction,hit_info,100.0));
    //Glancing hit at non planar quad
    direction = {-1.0,-1.0,0.0};
    origin = {1.0,1.0,1.0};
    direction.normalize();
    a = {-1.0,-1.0,-1.0};
    b = {0.0,-1.0,2.0};
    c = {0.0,2.0,-1.0};
    d = {0.0,1.0,1.0};
    ASSERT_TRUE(EngiGraph::rayQuadPatchIntersection(a,b,c,d,origin,direction,hit_info,100.0));
    //Facing towards twisted quad center
    direction = {-1.0,0.0,0.0};
    origin = {1.0,0.0,0.0};
    direction.normalize();
    a = {0.2,-1.0,1.0};
    b = {-0.2,-1.0,-1.0};
    c = {0.0,1.0,-1.0};
    d = {0.0,1.0,1.0};
    ASSERT_FALSE(EngiGraph::rayQuadPatchIntersection(a,b,c,d,origin,direction,hit_info,100.0));
    //Facing towards twisted quad non-center
    direction = {-1.0,0.0,0.0};
    origin = {1.0,0.1,0.0};
    direction.normalize();
    a = {0.2,-1.0,1.0};
    b = {-0.2,-1.0,-1.0};
    c = {0.0,1.0,-1.0};
    d = {0.0,1.0,1.0};
    ASSERT_TRUE(EngiGraph::rayQuadPatchIntersection(a,b,c,d,origin,direction,hit_info,100.0));
}