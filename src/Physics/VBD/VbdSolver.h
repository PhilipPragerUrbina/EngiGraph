//
// Created by Philip on 10/22/2023.
//

#pragma once
#include "src/Physics/Collisions/LinearPointCcd.h"
#include "src/FileIO/ObjLoader.h"
#include "src/Geometry/MeshConversions.h"

namespace EngiGraph {

    /**
     * Velocity based dynamics rigid-body solver.
     */
    class VBDSolver {
    public:
        struct Box {
            Eigen::Vector3d position = {0,0,0}, angular_velocity = {0,0,0}, velocity = {0,0,0};
            Eigen::Quaterniond rotation = Eigen::Quaterniond::Identity();
            double mass;
            Mesh mesh;
            Eigen::Vector3d dimensions;
            Eigen::Matrix3d inertia_tensor;
            Eigen::Vector3d force = {0,0,0};

            [[nodiscard]] Eigen::Matrix4d getRenderTransform() const {
                Eigen::Transform<double,3,Eigen::Affine> transform_initial = Eigen::Transform<double,3,Eigen::Affine>::Identity();
                transform_initial.translate(position).rotate(rotation).scale(dimensions).translate(Eigen::Vector3d{-0.5,-0.5,-0.5});
                return transform_initial.matrix();
            }

            Box(double mass, const Eigen::Vector3d& dimensions) : mass(mass), dimensions(dimensions){
                auto raw_mesh_cube = loadOBJ("./test_files/cube.obj");
                for (auto& vertex : raw_mesh_cube[0].vertices) {
                    vertex.position -= Eigen::Vector3f {0.5,0.5,0.5};
                    vertex.position = vertex.position.cwiseProduct( dimensions.cast<float>());
                }
                mesh = stripVisualMesh(raw_mesh_cube[0]);
                inertia_tensor = Eigen::Matrix3d::Zero();
                inertia_tensor.coeffRef(0,0) = 1.0/12.0 * mass * (dimensions.y() * dimensions.y() + dimensions.z() * dimensions.z());
                inertia_tensor.coeffRef(1,1) = 1.0/12.0 * mass * (dimensions.x() * dimensions.x() + dimensions.z() * dimensions.z());
                inertia_tensor.coeffRef(2,2) = 1.0/12.0 * mass * (dimensions.x() * dimensions.x() + dimensions.y() * dimensions.y());
            }

            Eigen::Matrix4d current_transform = Eigen::Matrix4d::Identity();
            Eigen::Matrix4d future_transform = Eigen::Matrix4d::Identity();

            void updateCurrentTransform(){
                Eigen::Transform<double,3,Eigen::Affine> transform_initial = Eigen::Transform<double,3,Eigen::Affine>::Identity();
                transform_initial.translate(position).rotate(rotation);
                current_transform = transform_initial.matrix();
            }

            Eigen::Vector3d getVelocityAtPoint(const Eigen::Vector3d& point){
                return velocity + angular_velocity.cross(point - position);
            }
            void addVelocityAtPoint(const Eigen::Vector3d& point, const Eigen::Vector3d& add_velocity){
                velocity += add_velocity;
                angular_velocity += (point - position).cross( add_velocity); //Center of gravity is the origin
            }

            void updateFutureTransform(double delta_time){
                Eigen::Transform<double,3,Eigen::Affine> transform_final = Eigen::Transform<double,3,Eigen::Affine>::Identity();
                Eigen::Quaterniond final_rotation = Eigen::Quaterniond(0, delta_time * 0.5 * angular_velocity.x(),  delta_time * 0.5  * angular_velocity.y(), delta_time * 0.5  * angular_velocity.z()) * rotation;
                final_rotation.vec() += rotation.vec();
                final_rotation.w() += rotation.w();
                final_rotation.normalize();
                transform_final.translate(position + velocity*delta_time).rotate(final_rotation);
                future_transform = transform_final.matrix();
            }

            void move(double delta_time){
                position += velocity*delta_time;
                Eigen::Quaterniond final_rotation = Eigen::Quaterniond(0, delta_time * 0.5 * angular_velocity.x(),  delta_time * 0.5  * angular_velocity.y(), delta_time * 0.5  * angular_velocity.z()) * rotation;
                final_rotation.vec() += rotation.vec();
                final_rotation.w() += rotation.w();
                final_rotation.normalize();
                rotation = final_rotation;
            }
        };

        std::vector<Box> bodies{};

        struct HitPair {
            std::vector<CCDHit> hits;
            int a, b;
        };

        void step(double delta_time){
            for (auto& body : bodies) {

                body.velocity += delta_time * body.force / body.mass;

                body.updateCurrentTransform();
                body.updateFutureTransform(delta_time);
            }

       //     std::vector<HitPair> hits;

        //    for (int j = 0; j < bodies.size(); ++j) {
           //     for (int k = j+1; k < bodies.size(); ++k) {
            //        auto pair_hits = linearCCD(bodies[j].mesh,bodies[k].mesh, bodies[j].current_transform, bodies[k].current_transform, bodies[j].future_transform,bodies[k].future_transform);
           //         if(!pair_hits.empty()){
           //             hits.push_back(HitPair{pair_hits,j,k});
           //         }
          //      }
          //  }

          //todo investigate nans propagating with scaled objects

            for (int j = 0; j < bodies.size(); ++j) {
                bool hit = false;
                std::vector<CCDHit> hits{};
                for (int k = 0; k < bodies.size(); ++k) {
                    if(j == k) continue;
                    auto pair_hits = linearCCD(bodies[j].mesh,bodies[k].mesh, bodies[j].current_transform, bodies[k].current_transform, bodies[j].future_transform,bodies[k].future_transform);
                    if(!pair_hits.empty()){
                        hits.insert(hits.end(), pair_hits.begin(), pair_hits.end());
                        hit = true;
                    }
                }
                if(!hit){
                    bodies[j].move(delta_time);
                }else{
                    for (int k = 0; k < 100; ++k) {
                        for (const auto& collision : hits) {
                            auto velocity_at_collision = bodies[j].getVelocityAtPoint(collision.global_point);
                            if(velocity_at_collision.dot(collision.normal_a_to_b) > 0.0){
                                auto bad_velocity = velocity_at_collision.dot(collision.normal_a_to_b) * collision.normal_a_to_b;
                                auto correction = -bad_velocity; //todo add relaxation multiplier
                                bodies[j].addVelocityAtPoint(collision.global_point,correction);
                            }
                        }
                    }
                    //solve
                }
            }


        }


    };

} // EngiGraph
