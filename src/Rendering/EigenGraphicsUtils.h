//
// Created by Philip on 9/18/2023.
//

#pragma once
#include <Eigen>
#include "../Units/AngleRadians.h"

namespace EngiGraph {

    /**
     * Create a perspective projection matrix.
     * @param fov Vertical field of view in radians. Must be greater than 0.0f;
     * @param aspect_ratio The aspect ratio. (Width/Height).
     * @param near_plane Near clipping plane.
     * @param far_plane Far clipping plane.
     * @return Perspective projection matrix.
     */
    Eigen::Matrix4f createPerspectiveProjection(AngleRadians<float> fov, float aspect_ratio, float near_plane, float far_plane);

    /**
     * Create a view matrix for a camera looking at a point.
     * @param origin Camera position.
     * @param target Where to look at.
     * @param up Upward vector.
     * @warning If origin == target or the camera direction == up, an identity matrix will be returned.
     * @return View matrix.
     */
    Eigen::Matrix4f createLookAt(const Eigen::Vector3f& origin, const Eigen::Vector3f& target, const Eigen::Vector3f& up);

} // EngiGraph
