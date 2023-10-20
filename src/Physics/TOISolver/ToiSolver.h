//
// Created by Philip on 10/9/2023.
//

#pragma once
#include "./src/Physics/Collisions/LinearPointCcd.h"
#include "src/Rendering/OpenGL/Resources/MeshResourceOgl.h"
#include "src/Rendering/OpenGL/Resources/TextureResourceOgl.h"
#include "src/Exceptions/RuntimeException.h"
#include <algorithm>
#include <iostream>

namespace EngiGraph {

    /**
     * Rigid body simulator that uses ccd time of impact and a special integration scheme to make intersection free dynamics.
     */
    class TOISolver {
    public:
        struct RigidBody{
            Eigen::Vector3d position;
            Eigen::Quaterniond rotation;
            Eigen::Vector3d velocity;
            Eigen::Vector3d angular_velocity;
            Eigen::Matrix4d initial_transform;
            Eigen::Matrix4d final_transform;


            std::shared_ptr<Mesh> collider;
            std::shared_ptr<MeshResourceOgl> render_mesh;
            std::shared_ptr<TextureResourceOgl> render_texture;
            double mass;
            Eigen::Matrix3d inertia_tensor;
            Eigen::Vector3d net_force;
            bool gravity = true;
        };

        struct Hit {
            double time;
            Eigen::Vector3d normal_a_to_b;
            Eigen::Vector3d point;
            uint32_t a,b;
        };

        static Eigen::Vector3d getVelocityAtPoint(const RigidBody& body, const Eigen::Vector3d& point){
            return body.velocity + body.angular_velocity.cross(point - body.position);
        }
        static void setVelocityAtPoint( RigidBody& body, const Eigen::Vector3d& point, const Eigen::Vector3d& velocity){
            body.velocity += velocity;
            body.angular_velocity += (point - body.position).cross( velocity);//Center of gravity is the origin
        }

        std::vector<RigidBody> bodies;

        Eigen::Vector3d gravity = {0.0,-9.8,0.0};
        void step(double delta_time){
            for (auto& body : bodies) {
                //force integration
                if(body.gravity){
                    body.velocity += gravity * delta_time;
                }
                body.rotation.normalize();
                Eigen::Transform<double,3,Eigen::Affine> transform_initial = Eigen::Transform<double,3,Eigen::Affine>::Identity();
                transform_initial.translate(body.position).rotate(body.rotation);
                body.initial_transform = transform_initial.matrix();

                Eigen::Transform<double,3,Eigen::Affine> transform_final = Eigen::Transform<double,3,Eigen::Affine>::Identity();
                Eigen::Quaterniond final_rotation = Eigen::Quaterniond(0, delta_time * 0.5 *body.angular_velocity.x(),  delta_time * 0.5 *body.angular_velocity.y(), delta_time * 0.5 *body.angular_velocity.z()) * body.rotation;
                final_rotation.vec() += body.rotation.vec();
                final_rotation.w() += body.rotation.w();
                final_rotation.normalize();
                transform_final.translate(body.position + body.velocity*delta_time).rotate(final_rotation);
                body.final_transform = transform_final.matrix();
            }

            double time_remaining = delta_time;

            std::vector<Hit> hits{};

            //only go through each pair once
            for (uint32_t body_a = 0; body_a < bodies.size(); ++body_a) {
                for (uint32_t body_b = body_a+1; body_b < bodies.size(); ++body_b) {
                    auto sub_hits = linearCCD(*bodies[body_a].collider, *bodies[body_b].collider, bodies[body_a].initial_transform,bodies[body_b].initial_transform,bodies[body_a].final_transform, bodies[body_b].final_transform);
                //    if(sub_hits && sub_hits->time < 1.0f){
                       // std::cout << body_a << " " << body_b << " i:" << 0 << " t: " << sub_hits->w() << "\n";
                    //    Eigen::Vector3d normal_a_to_b = sub_hits->normal_a_to_b;
                    //    hits.push_back(Hit{sub_hits->time,normal_a_to_b,sub_hits->global_point,body_a,body_b});
                 //   }
                }
            }
            //sort by time in ascending order
            std::sort(hits.begin(), hits.end(), [](const TOISolver::Hit& a, const TOISolver::Hit& b) {return a.time < b.time;});



            //todo fix wierd bug where the two spheres seem to be linked for no reason. Moving one affects the other in drastic ways despite not touching nor colliding.
            //It is almost like on collision with the cube, they get mixed up sometimes. For example, by default, the top sphere seems to react to the bottom sphere hitting the box.
            //Or speeding up the top sphere, causes the bottom sphere to glitch really fast when it hits the box.
            //This is clearly an implementation bug, as in theory these should not be affecting each other at all. Potentially some indices are getting mixed up somewhere?

            int i = 0;
            while (!hits.empty()){
                const double rollback_margin = time_remaining/100.0;
                i++;
                if(i > 1000) {
                    std::cout << "Warning: not able to properly resolve collision! \n";
                    break;
                }

                double t_last = hits[0].time;
                //careful this is not working right since the negatives are flipping
                for (int k = 0; k < 10; ++k) { //velocity solve iters
                    for (const auto& earliest : hits) {
                        if(abs(earliest.time - t_last) > rollback_margin/2.0){
                            break;
                        }
                     //   std::cout << earliest.normal_a_to_b << "n\n";
                        if(bodies[earliest.a].gravity){
                            Eigen::Vector3d velocity = getVelocityAtPoint(bodies[earliest.a],earliest.point);
                            velocity *= -1.0;
                            setVelocityAtPoint(bodies[earliest.a],earliest.point,velocity);
                        }
                        if(bodies[earliest.b].gravity){
                            Eigen::Vector3d velocity = getVelocityAtPoint(bodies[earliest.b],earliest.point);
                            velocity *= -1.0;
                            setVelocityAtPoint(bodies[earliest.b],earliest.point,velocity);
                        }
                    }
                }


                double current_time = t_last * time_remaining - rollback_margin;
                if(current_time <= 0.0){
                   // current_time = 0.0;
                }

                time_remaining -= current_time;
                if(time_remaining <= 0.0){
                    time_remaining = 0.0;
                    break;
                }

                for (auto& body : bodies) {
                    body.position += body.velocity * current_time;
                    Eigen::Quaterniond final_rotation = Eigen::Quaterniond(0, current_time * 0.5 *body.angular_velocity.x(),  current_time * 0.5 *body.angular_velocity.y(), current_time * 0.5 *body.angular_velocity.z()) * body.rotation;
                    final_rotation.vec() += body.rotation.vec();
                    final_rotation.w() += body.rotation.w();
                    final_rotation.normalize();
                    body.rotation = final_rotation;
                }

                for (auto& body : bodies) {
                    Eigen::Transform<double,3,Eigen::Affine> transform_initial = Eigen::Transform<double,3,Eigen::Affine>::Identity();
                    transform_initial.translate(body.position).rotate(body.rotation);
                    body.initial_transform = transform_initial.matrix();

                    Eigen::Transform<double,3,Eigen::Affine> transform_final = Eigen::Transform<double,3,Eigen::Affine>::Identity();
                    Eigen::Quaterniond final_rotation = Eigen::Quaterniond(0, time_remaining * 0.5 *body.angular_velocity.x(),  time_remaining * 0.5 *body.angular_velocity.y(), time_remaining * 0.5 *body.angular_velocity.z()) * body.rotation;
                    final_rotation.vec() += body.rotation.vec();
                    final_rotation.w() += body.rotation.w();
                    final_rotation.normalize();
                    transform_final.translate(body.position + body.velocity*time_remaining).rotate(final_rotation);
                    body.final_transform = transform_final.matrix();
                }

                hits.clear();

                for (uint32_t body_a = 0; body_a < bodies.size(); ++body_a) {
                    for (uint32_t body_b = body_a+1; body_b < bodies.size(); ++body_b) {
                        auto sub_hits = linearCCD(*bodies[body_a].collider, *bodies[body_b].collider, bodies[body_a].initial_transform,bodies[body_b].initial_transform,bodies[body_a].final_transform, bodies[body_b].final_transform);
                      //  if(sub_hits && sub_hits->time < 1.0f){
                        //    std::cout << body_a << " " << body_b << " i:" << i << " t: " << sub_hits->time << " p:" << sub_hits.value().global_point << "\n";
                            //todo what the heck this is passing back negative 0? This should not be passing back anything. It might have to do when one of the bodies is at rest. It only happens when the bodies are in line.
                        //    Eigen::Vector3d normal_a_to_b = sub_hits->normal_a_to_b; //todo get
                     //       hits.push_back(Hit{sub_hits->time,normal_a_to_b,sub_hits->global_point,body_a,body_b});
                        }
                    }
                }
                std::sort(hits.begin(), hits.end(), [](const TOISolver::Hit& a, const TOISolver::Hit& b) {return a.time < b.time;});
            }


            //todo Resolve collisions using normals, apply friction and new velocities
            //todo resolve multiple same time objects at once, and get multiple same time collision points


            //integrate until timestep ends
         //   for (auto& body : bodies) {
              //  body.position += body.velocity * time_remaining;
               // Eigen::Quaterniond final_rotation = Eigen::Quaterniond(0, time_remaining * 0.5 *body.angular_velocity.x(),  time_remaining * 0.5 *body.angular_velocity.y(), time_remaining * 0.5 *body.angular_velocity.z()) * body.rotation;
               // final_rotation.vec() += body.rotation.vec();
              //  final_rotation.w() += body.rotation.w();
              //  final_rotation.normalize();
               // body.rotation = final_rotation;
         //   }



      //  }

        //todo detect fast rotating objects and give multiple ccd substeps

        //todo only recompute ccd values if they are nearby and might be affected by the change

        //todo Separate groups of objects can be computed in parallel



    };

} // EngiGraph
