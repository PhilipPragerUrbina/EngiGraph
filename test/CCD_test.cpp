//
// Created by Philip on 10/8/2023.
//
#include "gtest/gtest.h"
#include "../src/Physics/Collisions/LinearPointCcd.h"
#include "../src/Physics/Collisions/LinearPointCcd.cpp"
#include "src/FileIO/ObjLoader.h"
#include "src/Geometry/MeshConversions.h"

//todo create standardized reliable way to test floating point eigen vectors to ensure deterministic testing

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

TEST(INTERSECTION_TESTS, TEST_LINEAR_CCD) {
    //Test helper functions
    {
        Eigen::Vector3d a {0.0,0.0,0.0};
        Eigen::Vector3d b {1.0,0.0,0.0};
        Eigen::Vector3d c {1.0,1.0,0.0};
        auto normal = EngiGraph::getNormal(a,b,c);
        Eigen::Vector3d expected = {0.0,0.0,-1.0};
        ASSERT_EQ(normal,expected);
    }
    {
        //normal between equal edges
        Eigen::Vector3d a_1 {1,0,0};
        Eigen::Vector3d a_2 {-1,0,0};
        Eigen::Vector3d b_1 {1,0,0};
        Eigen::Vector3d b_2 {-1,0,0};
        auto normal = EngiGraph::getNormalEdgeToEdge(a_1,a_2,b_1,b_2);
        Eigen::Vector3d expected = {1.0,0.0,0.0}; //Arbitrary normal
        ASSERT_EQ(normal,expected);
    }
    {
        //normal between parallel edges
        Eigen::Vector3d a_1 {1,-5,5};
        Eigen::Vector3d a_2 {-1,-5,5};
        Eigen::Vector3d b_1 {1,5,-5};
        Eigen::Vector3d b_2 {-1,5,-5};
        auto normal = EngiGraph::getNormalEdgeToEdge(a_1,a_2,b_1,b_2);
        Eigen::Vector3d expected = {0.0,0.707107,-0.707107}; //Arbitrary normal
        ASSERT_TRUE((expected-normal).norm() < 0.0001);
    }
    {
        //normal between edges
        Eigen::Vector3d a_1 {1,0,0};
        Eigen::Vector3d a_2 {-1,0,0};
        Eigen::Vector3d b_1 {0,1,0};
        Eigen::Vector3d b_2 {0,-1,0};
        auto normal = EngiGraph::getNormalEdgeToEdge(a_1,a_2,b_1,b_2);
        Eigen::Vector3d expected = {0.0,0.0,1.0}; //Polarity does not matter
        ASSERT_EQ(normal,expected);
    }

    //load resources
    auto raw_mesh_cube = EngiGraph::loadOBJ("./test_files/cube.obj");
    auto mesh_cube = EngiGraph::stripVisualMesh(raw_mesh_cube[0]);

    {
        //A cube hits a stationary cube as it moves down the z axis.
        //This is considered to be an edge case, as it contains many exactly aligning edges and points, such as parallel edges.
        Eigen::Transform<double, 3, Eigen::Affine> transform_a_initial = Eigen::Transform<double, 3, Eigen::Affine>::Identity();
        transform_a_initial.translate(Eigen::Vector3d{0.0, 0.0, 5.0});
        Eigen::Transform<double, 3, Eigen::Affine> transform_a_final = Eigen::Transform<double, 3, Eigen::Affine>::Identity();
        transform_a_final.translate(Eigen::Vector3d{0.0, 0.0, -5.0});
        Eigen::Transform<double, 3, Eigen::Affine> transform_b_initial = Eigen::Transform<double, 3, Eigen::Affine>::Identity();

        Eigen::Transform<double, 3, Eigen::Affine> transform_b_final = Eigen::Transform<double, 3, Eigen::Affine>::Identity();

        auto result = EngiGraph::linearCCD(mesh_cube, mesh_cube, transform_a_initial.matrix(),
                                           transform_b_initial.matrix(), transform_a_final.matrix(),
                                           transform_b_final.matrix());

        ASSERT_EQ(result.size(),
                  12); //12 hits should be registered. 4 points from a hitting b. 4 points from b hitting a. And 4 edges hitting each other.
        for (const auto &hit: result) {
            //All the normals should face downward, as a is above b at collision
            ASSERT_TRUE(hit.normal_a_to_b.isApprox(Eigen::Vector3d{0.0, 0.0, -1.0}));
            //Hits must be within the top plane of the cube
            ASSERT_DOUBLE_EQ(hit.global_point.z(), 1.0);
            ASSERT_TRUE(hit.global_point.x() <= 1.0 && hit.global_point.x() >= 0.0);
            ASSERT_TRUE(hit.global_point.y() <= 1.0 && hit.global_point.y() >= 0.0);
            //hits must be at time 4/10 (cube traveled 4 units out of 10 units down before colliding)
            ASSERT_DOUBLE_EQ(hit.time, 0.4);
        }
    }

    {
        //A cube hits a stationary cube as it moves down the x axis by a very small amount.
        //This is a variation on the last test
        Eigen::Transform<double, 3, Eigen::Affine> transform_a_initial = Eigen::Transform<double, 3, Eigen::Affine>::Identity();
        transform_a_initial.translate(Eigen::Vector3d{1.001, 0.0, 0.0});
        Eigen::Transform<double, 3, Eigen::Affine> transform_a_final = Eigen::Transform<double, 3, Eigen::Affine>::Identity();
        transform_a_final.translate(Eigen::Vector3d{0.999, 0.0, 0.0});
        Eigen::Transform<double, 3, Eigen::Affine> transform_b_initial = Eigen::Transform<double, 3, Eigen::Affine>::Identity();

        Eigen::Transform<double, 3, Eigen::Affine> transform_b_final = Eigen::Transform<double, 3, Eigen::Affine>::Identity();

        auto result = EngiGraph::linearCCD(mesh_cube, mesh_cube, transform_a_initial.matrix(),
                                           transform_b_initial.matrix(), transform_a_final.matrix(),
                                           transform_b_final.matrix());

        ASSERT_EQ(result.size(),
                  12); //12 hits should be registered. 4 points from a hitting b. 4 points from b hitting a. And 4 edges hitting each other.
        for (const auto &hit: result) {
            //All the normals should face downward, as a is above b at collision
            ASSERT_TRUE(hit.normal_a_to_b.isApprox(Eigen::Vector3d{-1.0, 0.0, 0.0}));
            //Hits must be within the top plane of the cube
            ASSERT_DOUBLE_EQ(hit.global_point.x(), 1.0);
            ASSERT_TRUE(hit.global_point.z() <= 1.0 && hit.global_point.z() >= 0.0);
            ASSERT_TRUE(hit.global_point.y() <= 1.0 && hit.global_point.y() >= 0.0);
            //hits must be at time 0.001/0.002 (cube traveled 4 units out of 10 units down before colliding)
            ASSERT_TRUE(abs(hit.time -  0.5) < 0.00001); //slight error is okay
        }
    }
    {
        //A cube does NOT hit a stationary cube as it moves down the x axis by a very small amount.
        Eigen::Transform<double, 3, Eigen::Affine> transform_a_initial = Eigen::Transform<double, 3, Eigen::Affine>::Identity();
        transform_a_initial.translate(Eigen::Vector3d{1.001, 0.0, 0.0});
        Eigen::Transform<double, 3, Eigen::Affine> transform_a_final = Eigen::Transform<double, 3, Eigen::Affine>::Identity();
        transform_a_final.translate(Eigen::Vector3d{1.0001, 0.0, 0.0});
        Eigen::Transform<double, 3, Eigen::Affine> transform_b_initial = Eigen::Transform<double, 3, Eigen::Affine>::Identity();

        Eigen::Transform<double, 3, Eigen::Affine> transform_b_final = Eigen::Transform<double, 3, Eigen::Affine>::Identity();

        auto result = EngiGraph::linearCCD(mesh_cube, mesh_cube, transform_a_initial.matrix(),
                                           transform_b_initial.matrix(), transform_a_final.matrix(),
                                           transform_b_final.matrix());

        ASSERT_EQ(result.size(), 0);
    }

    {
        //A cube hits a stationary cube as it moves down the z axis this time offset on the x axis.
        Eigen::Transform<double, 3, Eigen::Affine> transform_a_initial = Eigen::Transform<double, 3, Eigen::Affine>::Identity();
        transform_a_initial.translate(Eigen::Vector3d{0.1, 0.0, 5.0});
        Eigen::Transform<double, 3, Eigen::Affine> transform_a_final = Eigen::Transform<double, 3, Eigen::Affine>::Identity();
        transform_a_final.translate(Eigen::Vector3d{0.1, 0.0, -5.0});
        Eigen::Transform<double, 3, Eigen::Affine> transform_b_initial = Eigen::Transform<double, 3, Eigen::Affine>::Identity();

        Eigen::Transform<double, 3, Eigen::Affine> transform_b_final = Eigen::Transform<double, 3, Eigen::Affine>::Identity();

        auto result = EngiGraph::linearCCD(mesh_cube, mesh_cube, transform_a_initial.matrix(),
                                           transform_b_initial.matrix(), transform_a_final.matrix(),
                                           transform_b_final.matrix());

        ASSERT_EQ(result.size(), 10); //4 points and two edges no longer contact. 4 new edge contacts present.
        for (const auto &hit: result) {
            //All the normals should face in the downward direction, as a is above b at collision
            ASSERT_TRUE(hit.normal_a_to_b.dot(Eigen::Vector3d{0.0, 0.0, -1.0}) > 0.5);
            //Hits must be within the top plane of the cube
            ASSERT_DOUBLE_EQ(hit.global_point.z(), 1.0);
            ASSERT_TRUE(hit.global_point.x() <= 1.0 && hit.global_point.x() >= 0.0);
            ASSERT_TRUE(hit.global_point.y() <= 1.0 && hit.global_point.y() >= 0.0);
            //hits must be at time 4/10 (cube traveled 4 units out of 10 units down before colliding)
            ASSERT_DOUBLE_EQ(hit.time, 0.4);
        }
    }


    //todo false tests
    //todo true tests
    //todo edge case tests
    //todo real world tests
    //todo targeted tests two way.
    //todo test strange case with the ghost collisions with the spheres

}


