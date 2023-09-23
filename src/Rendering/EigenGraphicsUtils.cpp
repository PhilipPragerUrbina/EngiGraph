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
    createLookAtView(const Eigen::Vector3f &eye, const Eigen::Vector3f &target, const Eigen::Vector3f &up) {
        //Calculate local directions
        Eigen::Vector3f forward_axis = (eye - target).normalized();
        Eigen::Vector3f right_axis = up.cross(forward_axis).normalized();
        Eigen::Vector3f up_axis = forward_axis.cross(right_axis);
        //Great explanation: https://medium.com/@carmencincotti/lets-look-at-magic-lookat-matrices-c77e53ebdf78
        Eigen::Matrix4f matrix;
        matrix << //This is actually the inverse of the lookAt matrix: The view matrix.
                right_axis.x() , up_axis.x() , forward_axis.x() , 0,
                right_axis.y() , up_axis.y() , forward_axis.y() , 0,
                right_axis.z() , up_axis.z() , forward_axis.z() , 0,
                -(eye.dot(right_axis)) , -(eye.dot(up_axis)) , -(eye.dot(forward_axis)) , 1;
        return matrix;
    }


} // EngiGraph