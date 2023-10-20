//
// Created by Philip on 10/4/2023.
//
#include <gtest/gtest.h>
#include "../src/Geometry/MeshUtilities.h"
#include "../src/Geometry/MeshConversions.h"
TEST(UTILITY_TESTS, TEST_MESH_REDUCTION) {
    //two conjoined triangles
    std::vector<Eigen::Vector3f> original_vertices = {{0.0f,0.0f,0.0f},{1.0f,1.0f,1.0f},{1.0f,-1.0f,-1.0f},
                                                      {0.0f,0.0f,0.0f},{1.0f,1.0f,1.0f},{5.0f,5.0f,1.0f} };
    std::vector<uint32_t> original_indices = {0,1,2,3,4,5};

    auto mesh = EngiGraph::Mesh{original_vertices,original_indices};

    auto reduced_mesh = EngiGraph::reduceMesh(mesh);

    std::vector<Eigen::Vector3f> expected_vertices = {{0.0f,0.0f,0.0f},{1.0f,1.0f,1.0f},{1.0f,-1.0f,-1.0f},{5.0f,5.0f,1.0f} };
    std::vector<uint32_t> expected_indices = {0,1,2,0,1,3};

    ASSERT_EQ(expected_indices.size(),reduced_mesh.triangle_indices.size());
    ASSERT_EQ(expected_vertices.size(),reduced_mesh.vertices.size());
    ASSERT_EQ(10,reduced_mesh.edge_indices.size());

    //Check that indices are correct
    for (int j = 0; j < expected_indices.size(); ++j) {
        ASSERT_EQ(expected_indices[j], reduced_mesh.triangle_indices[j]);
    }

    //check vertices are correct
    for (int j = 0; j < expected_vertices.size(); ++j) {
        ASSERT_EQ(expected_vertices[j], reduced_mesh.vertices[j]);
    }
}

TEST(UTILITY_TESTS, TEST_MESH_CONVERSION) {
    //visual triangle
    std::vector<EngiGraph::VisualMesh::Vertex> original_vertices = {{{0.0f,0.0f,0.0f},{1.0f,0.0f,0.0f},{0.0f,0.0f}},{{1.0f,0.0f,0.0f},{1.0f,0.0f,0.0f},{0.0f,0.0f}},{{1.0f,1.0f,0.0f},{1.0f,0.0f,0.0f},{0.0f,0.0f}}};
    std::vector<uint32_t> original_indices = {0,1,2};

    auto visual_mesh = EngiGraph::VisualMesh{"visual mesh",original_vertices,original_indices};

    auto base_mesh = EngiGraph::stripVisualMesh(visual_mesh);

    std::vector<Eigen::Vector3f> expected_vertices = {{0.0f,0.0f,0.0f},{1.0f,0.0f,0.0f},{1.0f,1.0f,0.0f} }; //only positions
    std::vector<uint32_t> expected_indices = {0,1,2};

    ASSERT_EQ(expected_indices.size(),base_mesh.triangle_indices.size());
    ASSERT_EQ(expected_vertices.size(),base_mesh.vertices.size());

    //Check that indices are correct
    for (int j = 0; j < expected_indices.size(); ++j) {
        ASSERT_EQ(expected_indices[j], base_mesh.triangle_indices[j]);
    }

    //check vertices are correct
    for (int j = 0; j < expected_vertices.size(); ++j) {
        ASSERT_EQ(expected_vertices[j], base_mesh.vertices[j]);
    }
}