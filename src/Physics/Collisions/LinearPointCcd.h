//
// Created by Philip on 10/4/2023.
//

#pragma once
#include <Eigen>
#include <optional>
#include "../../Geometry/Mesh.h"

namespace EngiGraph {

    /**
     * Perform linear continuous collision detection on two objects.
     * @param a,b Local Geometry of both objects.
     * @param a_initial,b_initial Global transforms at start of time_step.
     * @param a_final,b_final Global target positions at end of time step.
     * @param time Time between initial and final position.
     * @return Earliest global collision point, with time relative to start position(w). Or none if no collision.
     * todo edge detection. Currently meshes with fewer vertices can miss collisions.
     * todo BVH acceleration structure.
     * todo return multiple simultaneous contact points.
     */
    std::optional<Eigen::Vector4d> linearCCD(const Mesh& a, const Mesh& b, const Eigen::Matrix4d& a_initial, const Eigen::Matrix4d& b_initial,const Eigen::Matrix4d& a_final, const Eigen::Matrix4d& b_final, const double& time);

} // EngiGraph
