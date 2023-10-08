//
// Created by Philip on 10/4/2023.
//

#include "LinearPointCcd.h"
#include <vector>
#include <iostream>

namespace EngiGraph {

    //todo test and optimize
    //todo clean up
    //todo replace with Watertight ray triangle intersection
    bool rayTriangleIntersection(const Eigen::Vector3d& a, const Eigen::Vector3d& b, const Eigen::Vector3d& c, const Eigen::Vector3d& origin, const Eigen::Vector3d& direction, double& distance ){
        Eigen::Vector3d v0v1 = b - a;
        Eigen::Vector3d v0v2 = c - a;
        Eigen::Vector3d pvec = direction.cross(v0v2);
        double det = v0v1.dot(pvec);

        //floating point error range. Larger for larger objects to avoid speckling problem.
        const double EPSILON = 0.00000f; //todo check
        if (std::abs(det) < EPSILON) return false;

        Eigen::Vector3d barycentric_coordinates;

        double invdet = 1.0 / det;
        Eigen::Vector3d tvec = origin - a;
        barycentric_coordinates[0] = tvec.dot(pvec) * invdet;
        if (barycentric_coordinates[0] < 0.0f || barycentric_coordinates[0] > 1.0f) return false;

        Eigen::Vector3d qvec = tvec.cross(v0v1);
        barycentric_coordinates[1] = direction.dot(qvec) * invdet;
        if (barycentric_coordinates[1] < 0.0f || barycentric_coordinates[0] + barycentric_coordinates[1] > 1.0f) return false;

        barycentric_coordinates[2] = 1.0f - barycentric_coordinates[0] - barycentric_coordinates[1];

        double dist = v0v2.dot(qvec) * invdet;
        const double DELTA = 0.0001; //check if in small range. this is to stop ray from intersecting with triangle again after bounce.
        //todo check
        if (dist > DELTA){
            distance = dist;
            return true;
        }
        return false;
    }

    //Do linear ccd treating one mesh as a stationary triangle mesh and the other as moving points.
    std::optional<Eigen::Vector4d> linearCCDOneWay(const Mesh &tri_mesh, const Mesh &point_mesh, const Eigen::Matrix4d &point_mesh_initial,  const Eigen::Matrix4d &point_mesh_final,const double & time){
        double earliest = 1000000.0f;
        std::optional<Eigen::Vector3d> earliest_point;
        for (const auto& local_point : point_mesh.vertices) {
            auto local_point_4 = Eigen::Vector4d (local_point.x(),local_point.y(),local_point.z(),1.0f); //todo optimize
            Eigen::Vector3d point_initial = (point_mesh_initial * local_point_4).head<3>();
            Eigen::Vector3d point_final = (point_mesh_final * local_point_4).head<3>();
std::cout << point_initial << " & "<< point_final << " |\n";

            double speed = (point_final - point_initial).norm() / time;
            if(speed < 0.000001){
                continue;
            }
            Eigen::Vector3d direction = (point_final - point_initial).normalized();


            for (int triangle_index = 0; triangle_index < tri_mesh.triangle_indices.size(); triangle_index += 3) {
               //ray triangle intersection is great for our needs
               //We do not care about what happened in the past, only the future, so a ray makes sense
               double distance = 0.0f;
               //todo check precision
               if(rayTriangleIntersection(tri_mesh.vertices[tri_mesh.triangle_indices[triangle_index+0]].cast<double>(),tri_mesh.vertices[tri_mesh.triangle_indices[triangle_index+1]].cast<double>(),tri_mesh.vertices[tri_mesh.triangle_indices[triangle_index+2]].cast<double>(),
                                          point_initial,direction,distance)){
                   double time_of_impact = distance / speed;
                   if(time_of_impact < earliest){
                       earliest = time_of_impact;
                       earliest_point = point_initial + direction * distance;
                   }
               }
            }
        }
        if(earliest_point){
            return Eigen::Vector4d {earliest_point.value().x(),earliest_point.value().y(),earliest_point.value().z(),earliest};
        }
        return {};
    }

   std::optional<Eigen::Vector4d> linearCCD(const Mesh &a, const Mesh &b, const Eigen::Matrix4d &a_initial, const Eigen::Matrix4d &b_initial,
              const Eigen::Matrix4d &a_final, const Eigen::Matrix4d &b_final, const double &time) {
        //A stationary
        std::optional<Eigen::Vector4d> b_to_a = linearCCDOneWay(a,b, a_initial.inverse() * b_initial,a_final.inverse()*b_final,time); //todo check if multiplication needs to be switched
        std::optional<Eigen::Vector4d> a_to_b = linearCCDOneWay(b,a, b_initial.inverse() * a_initial,b_final.inverse()*a_final,time);


        //convert to global space
        if(b_to_a){
            Eigen::Vector4d b_to_a_no_time = b_to_a.value();
            b_to_a_no_time.w() = 1.0f;
            b_to_a_no_time = (a_initial * b_to_a_no_time);
            b_to_a_no_time.w() = b_to_a->w();
            b_to_a = b_to_a_no_time;
        }
       if(a_to_b){
           Eigen::Vector4d a_to_b_no_time = a_to_b.value();
           a_to_b_no_time.w() = 1.0f;
           a_to_b_no_time = (b_initial * a_to_b_no_time);
           a_to_b_no_time.w() = a_to_b->w();
           a_to_b = a_to_b_no_time;
       }


        //todo optimize
        // Get the earliest time
        if(!b_to_a && !a_to_b){
            return {};
        }
        if(!b_to_a && a_to_b){
            return a_to_b;
        }
        if(b_to_a && !a_to_b){
            return b_to_a;
        }
        if(b_to_a->w() < a_to_b->w()){
            return b_to_a;
        }else{
            return a_to_b;
        }
    }

} // EngiGraph