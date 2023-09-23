//
// Created by Philip on 9/18/2023.
//
#include <gtest/gtest.h>
#include "../src/FileIO/ObjLoader.h"
#include "../src/Exceptions/RuntimeException.h"
TEST(LOADER_TESTS, TEST_OBJ_LOADER) {
    //test loading in a cube
    std::vector<EngiGraph::VisualMesh> meshes;
    ASSERT_NO_THROW(meshes = EngiGraph::loadOBJ("test_files/cube.obj")); //test successful load
    ASSERT_EQ(meshes.size(),1); //test shapes
    ASSERT_EQ(meshes[0].getTriangleCount(), 12); //test count
    //test auto-generated normals
    Eigen::Vector3f total_norm(0,0,0);
    float total_magnitude = 0;
    for (const auto& vertex : meshes[0].vertices) {
        total_norm += vertex.normal;
        total_magnitude += vertex.normal.norm();
    }
    ASSERT_FLOAT_EQ(total_magnitude,36.0f); //the sum should be 36 since there is a normal for each 12 vertex in 3 directions.
    ASSERT_FLOAT_EQ(total_norm.norm(),0.0f); //the sum should be 0 since even faces cancel out.

    ASSERT_STREQ(meshes[0].name.c_str(), "cube");
}