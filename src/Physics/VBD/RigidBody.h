//
// Created by Philip on 10/22/2023.
//

#pragma once

#include <utility>

#include "src/Geometry/Mesh.h"

namespace EngiGraph {

    /**
     * A simple unconstrained rigid-body, including a collider.
     */
    class RigidBody {
        //todo split impl to cpp
    public:
        //Shape info
        std::shared_ptr<Mesh> collider;
        double mass;
        Eigen::Matrix3d inertia_tensor;
        Eigen::Vector3d center_of_mass;

        //Motion info
        Eigen::Vector3d position = {0,0,0}, velocity = {0,0,0}, angular_velocity= {0,0,0};
        Eigen::Quaterniond rotation = Eigen::Quaterniond ::Identity();
    private:
        /**
         * Get the inertia tensor for a unit mass point.
         * @param r Point.
         * @return Moment of inertia.
         * @details https://stackoverflow.com/questions/67078659/how-can-i-calculate-the-inertia-tensor-of-a-hollow-object-defined-by-a-triangle
         */
        static Eigen::Matrix3d inertiaTensorPoint(Eigen::Vector3d r,double mass)
        {
            Eigen::Matrix3d i{};
            i << r.y()*r.y() + r.z()*r.z(), -r.x()*r.y(), -r.x()*r.z(),
                 -r.x()*r.y(), r.x()*r.x()+ r.z()*r.z(), -r.y()*r.z(),
                 -r.x()*r.z(), -r.y()*r.z(), r.x()*r.x() + r.y()*r.y();
            return mass * i ;
        }
    public:
        /**
         * Create a rigid-body from a mesh.
         * Will calculate a hollow inertia tensor and mass, as well as center of mass.
         * @param collider Collider mesh.
         * @param density Density of mesh in mass per unit cubed.
         * @param thickness Thickness of triangle "panels".
         */
        RigidBody(std::shared_ptr<Mesh> collider, double density, double thickness) : collider(collider){
            mass = 0;
            center_of_mass = {0,0,0};
            inertia_tensor = Eigen::Matrix3d::Zero();

            //todo add support for scaling
            for (int j = 0; j < collider->triangle_indices.size(); j+=3) {
                Eigen::Vector3d a = collider->vertices[collider->triangle_indices[j+0]].cast<double>();
                Eigen::Vector3d b = collider->vertices[collider->triangle_indices[j+1]].cast<double>();
                Eigen::Vector3d c = collider->vertices[collider->triangle_indices[j+2]].cast<double>();
                //all vertices have the same mass so center of mass is in center of triangle.
                Eigen::Vector3d triangle_center_of_mass = (a + b + c) / 3.0;
                //Calculate triangle mass
                double triangle_area = 0.5 * (b - a).cross(c - a).norm();
                double triangle_mass = triangle_area * density * thickness;
                mass += triangle_mass;
                center_of_mass += triangle_center_of_mass * triangle_mass;
            }
            center_of_mass /= mass; //Weighted average

            for (int j = 0; j < collider->triangle_indices.size(); j+=3) {
                //relative to center of mass
                Eigen::Vector3d a = collider->vertices[collider->triangle_indices[j+0]].cast<double>() - center_of_mass;
                Eigen::Vector3d b = collider->vertices[collider->triangle_indices[j+1]].cast<double>() - center_of_mass;
                Eigen::Vector3d c = collider->vertices[collider->triangle_indices[j+2]].cast<double>() - center_of_mass;
                //Calculate triangle mass
                double triangle_area = 0.5 * (b - a).cross(c - a).norm();
                double triangle_mass = triangle_area * density * thickness;
                Eigen::Matrix3d triangle_inertia_tensor = inertiaTensorPoint(a,triangle_mass/3.0)+inertiaTensorPoint(b,triangle_mass/3.0)+inertiaTensorPoint(c,triangle_mass/3.0);
                inertia_tensor += triangle_inertia_tensor;
            }

        }
    };

} // EngiGraph
