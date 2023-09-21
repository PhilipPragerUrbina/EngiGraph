//
// Created by Philip on 9/18/2023.
//

#include "EigenGraphicsUtils.h"

namespace EngiGraph {

    Eigen::Matrix4f
    createPerspectiveProjection(AngleRadians<float> fov, float aspect_ratio, float near_plane, float far_plane) {
        float y_scale = cosf(fov.value/2.0f) / sinf(fov.value/2.0f);
        float x_scale = y_scale/aspect_ratio;

        Eigen::Matrix4f projection;
        projection <<
        x_scale , 0 , 0 , 0,
        0 , y_scale , 0 , 0,
        0 , 0 , -(far_plane+near_plane)/(far_plane-near_plane), -1,
        0 , 0 , -2.0f*near_plane*far_plane/(far_plane-near_plane) , 0;

        return projection;
    }

    Eigen::Matrix4f
    createLookAt(const Eigen::Vector3f &origin, const Eigen::Vector3f &target, const Eigen::Vector3f &up) {
        return Eigen::Matrix4f();
    }


} // EngiGraph