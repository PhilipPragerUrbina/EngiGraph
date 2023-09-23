//
// Created by Philip on 9/18/2023.
//

#include "Camera.h"

namespace EngiGraph {

    void Camera::updateView() {
        view_matrix = createLookAtView(position,look_target,up);
    }

    void Camera::updateProjection() {
        projection_matrix = createPerspectiveProjection(convert(field_of_view), aspect_ratio, near_clipping_plane_z, far_clipping_plane_z);
    }

    void Camera::setAspectRatio(float new_aspect_ratio) {
        aspect_ratio = new_aspect_ratio;
        updateProjection();
    }

    void Camera::setFOV(AngleDegrees<float> new_field_of_view) {
        field_of_view = new_field_of_view;
        updateProjection();
    }

    Camera::Camera(const AngleDegrees<float>& field_of_view, float aspect_ratio, float near_clipping_plane_z,
                   float far_clipping_plane_z, const Eigen::Vector3f& up) : field_of_view(field_of_view),
                                                                            aspect_ratio(aspect_ratio),
                                                                            near_clipping_plane_z(
                                                                                    near_clipping_plane_z),
                                                                            far_clipping_plane_z(far_clipping_plane_z),
                                                                            up(up), position(1,1,1), look_target(0,0,0) {
        updateProjection();
        updateView();
    }

    void Camera::setPosition(const Eigen::Vector3f &new_position) {
        position = new_position;
        updateView();
    }

    void Camera::setLookTarget(const Eigen::Vector3f &new_look_target) {
        look_target = new_look_target;
        updateView();
    }
} // EngiGraph