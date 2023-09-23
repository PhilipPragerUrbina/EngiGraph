//
// Created by Philip on 9/18/2023.
//

#pragma once
#include "EigenGraphicsUtils.h"
#include "../Units/AngleConversions.h"
namespace EngiGraph {

    /**
     * Represents a 3D camera for rendering.
     */
    class Camera {
    private:
        //Computed values
        Eigen::Matrix4f view_matrix;
        Eigen::Matrix4f projection_matrix;

        //Given values(Projection)
        AngleDegrees<float> field_of_view;
        float aspect_ratio;
        float near_clipping_plane_z;
        float far_clipping_plane_z;

        //Given values(View)
        Eigen::Vector3f up;
        Eigen::Vector3f look_target;
        Eigen::Vector3f position;

        /**
         * Recalculate view matrix.
         */
        void updateView();

        /**
         * Recalculate projection matrix.
         */
        void updateProjection();

    public:

        /**
         * Create a perspective camera
         * @details Position is initialized to (1,1,1) and target to (0,0,0)
         * @param field_of_view Initial field of view in degrees.
         * @param aspect_ratio Initial aspect ratio(W/H)
         * @param near_clipping_plane_z Near clipping plane distance.
         * @param far_clipping_plane_z Far clipping plane distance.
         * @param up Global up unit vector.
         */
        Camera(const AngleDegrees<float> &field_of_view, float aspect_ratio, float near_clipping_plane_z,
               float far_clipping_plane_z, const Eigen::Vector3f &up);

        /**
         * Set the camera position and calculate new view matrix.
         * @param new_position New eye position in global space.
         */
        void setPosition(const Eigen::Vector3f& new_position);

        /**
         * Set the camera's target and calculate new view matrix.
         * @param new_look_target New point to look towards in global space.
         */
        void setLookTarget(const Eigen::Vector3f& new_look_target);

        /**
         * Set the aspect ratio and calculate new projection matrix.
         * @param new_aspect_ratio New aspect ratio. (width/height)
         */
        void setAspectRatio(float new_aspect_ratio);

        /**
         * Set the field of view and calculate new projection matrix.
         * @param new_field_of_view New field of view in degrees.
         */
        void setFOV(AngleDegrees<float> new_field_of_view);


        /**
         * Get the view matrix.
         */
        [[nodiscard]] const Eigen::Matrix4f &getViewMatrix() const {
            return view_matrix;
        }

        /**
         * Get the perspective projection matrix.
         */
        [[nodiscard]] const Eigen::Matrix4f &getProjectionMatrix() const {
            return projection_matrix;
        }

        /**
         * Get the field of view in degrees.
         */
        [[nodiscard]] const AngleDegrees<float> &getFieldOfView() const {
            return field_of_view;
        }

        /**
         * Get the near clipping plane distance.
         */
        [[nodiscard]] float getNearClippingPlaneZ() const {
            return near_clipping_plane_z;
        }

        /**
         * Get the far clipping plane distance.
         */
        [[nodiscard]] float getFarClippingPlaneZ() const {
            return far_clipping_plane_z;
        }

        /**
         * Get the point the camera is looking at in global space.
         */
        [[nodiscard]] const Eigen::Vector3f &getLookTarget() const {
            return look_target;
        }

        /**
         * Get the position of the camera eye in global space.
         */
        [[nodiscard]] const Eigen::Vector3f &getPosition() const {
            return position;
        }


    };

} // EngiGraph
