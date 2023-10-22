//
// Created by Philip on 10/4/2023.
//

#pragma once
#include <Eigen>
#include "./src/Geometry/Mesh.h"

namespace EngiGraph {

    /**
     * Hit information for a single CCD hit point.
     */
    struct CCDHit{
        /**
         * Time of collision relative to the start CCD position.
         */
        double time;
        /**
         * The global position of the collision point.
         */
        Eigen::Vector3d global_point;
        /**
         * The global normal of the contact, facing b.
         */
        Eigen::Vector3d normal_a_to_b;
    };

    /**
     * Perform linear continuous collision detection on two meshes.
     * @param a,b Local Geometry of both objects.
     * @param a_initial,b_initial Global transforms at start of time_step.
     * @param a_final,b_final Global target positions at end of time step.
     * @details Time is 0 at initial and 1 at final.
     * @warning Since this linearly interpolates the movement of vertices, some amount of deformation may happen if transformation rotates points.
     * @return Earliest collision. Will return multiple collision points if they happen at the same time.
     * @details Collision points can include edge to edge, and point to face.
     * //todo add time delta, point combine delta, and normal rollback as options or constants
     * @details To avoid a bunch of duplicate collision points, collision points that happen at the same time in very proximity are averaged into a single point.
     */
    std::vector<CCDHit> linearCCD(const Mesh& a, const Mesh& b, const Eigen::Matrix4d& a_initial, const Eigen::Matrix4d& b_initial,const Eigen::Matrix4d& a_final, const Eigen::Matrix4d& b_final);

} // EngiGraph
