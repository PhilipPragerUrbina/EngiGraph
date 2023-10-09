//
// Created by Philip on 10/4/2023.
//

#pragma once
#include <Eigen>
#include <optional>
#include "./src/Geometry/Mesh.h"

namespace EngiGraph {

    /**
     * Perform linear continuous collision detection on two meshes.
     * @param a,b Local Geometry of both objects.
     * @param a_initial,b_initial Global transforms at start of time_step.
     * @param a_final,b_final Global target positions at end of time step.
     * @details Time is 0 at initial and 1 at final.
     * @warning Since this linearly interpolates the movement of vertices, some amount of deformation may happen if transformation rotates points.
     * @return Earliest global collision point, with time relative to start position(stored in w component). Or none if no collision.
     * //todo return multiple contact points
     */
    std::optional<Eigen::Vector4d> linearCCD(const Mesh& a, const Mesh& b, const Eigen::Matrix4d& a_initial, const Eigen::Matrix4d& b_initial,const Eigen::Matrix4d& a_final, const Eigen::Matrix4d& b_final);

} // EngiGraph
