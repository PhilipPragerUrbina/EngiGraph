//
// Created by Philip on 10/22/2023.
//
#include "gtest/gtest.h"
#include "../../src/Physics/VBD/RigidBody.h"
#include "src/Geometry/MeshConversions.h"
#include "src/FileIO/ObjLoader.h"

TEST(PHYSICS_TESTS, TEST_INERTIA_TENSOR){
    auto raw_mesh_cube = EngiGraph::loadOBJ("./test_files/cube.obj");
    auto mesh_cube = EngiGraph::stripVisualMesh(raw_mesh_cube[0]);
    std::shared_ptr<EngiGraph::Mesh> mesh_cube_ptr = std::make_shared<EngiGraph::Mesh>(mesh_cube);
    auto rigidbody = EngiGraph::RigidBody(mesh_cube_ptr,1.0,0.1);

    ASSERT_TRUE(rigidbody.center_of_mass.isApprox(Eigen::Vector3d{0.5,0.5,0.5}));
    //Each face has surface area of 1*1, and the density is 1.
    ASSERT_DOUBLE_EQ(rigidbody.mass, 0.1 * 6.0);

    //row, column
    //todo fix
    //see https://physics.stackexchange.com/questions/105229/tensor-of-inertia-of-a-hollow-cube for how analytical values were calculated
    std::cout << rigidbody.inertia_tensor << "\n";
  //  ASSERT_NEAR(rigidbody.inertia_tensor.coeff(0,0), 5.0/3.0 * 0.1 * 1.0 , 0.0001); //5/3 ma^2

}