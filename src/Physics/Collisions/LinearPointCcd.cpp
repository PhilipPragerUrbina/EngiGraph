//
// Created by Philip on 10/4/2023.
//

#include "LinearPointCcd.h"
#include <vector>
#include <iostream>

namespace EngiGraph {



    /**
     * Calculate the dimensions where the ray direction is maximal.
     * @see rayTriangleIntersection()
     * @details This should be called once per ray, and then passed into the ray triangle intersection code.
     * @param direction Ray direction unit vector.
     * @return Vector containing kx,ky,kz. Each number represents one of 3 axis(0,1,2).
     */
    Eigen::Matrix<uint8_t,3,1> calculateRayDimensions(const Eigen::Vector3d& direction){
        uint8_t kz = 0;
        direction.cwiseAbs().maxCoeff(&kz);
        uint8_t kx = (kz + 1) % 3;
        uint8_t ky = (kx + 1) % 3;
        return {kx,ky,kz};
    }

    /**
     * Calculate ray shear constraints.
     * @see rayTriangleIntersection()
     * @details This should be called once per ray, and then passed into the ray triangle intersection code.
     * @param k From calculateRayDimensions() with same direction.
     * @warning Direction must be unit vector or nans will propagate.
     * @param direction Ray direction unit vector.
     * @return Vector containing sx,sy,sz.
     */
    Eigen::Vector3d calculateRayShearConstraints(const Eigen::Matrix<uint8_t,3,1>& k, const Eigen::Vector3d& direction){
        return {direction[k.x()]/direction[k.z()], direction[k.y()]/direction[k.z()], 1.0/direction[k.z()]};
    }


    /**
     * Watertight ray triangle intersection.
     * @details This does not perform backface culling.
     * @see https://jcgt.org/published/0002/01/05/paper.pdf
     * @param a, b, c Triangle vertices.
     * @param origin Ray origin.
     * @param hit_info Output vector containing information about hit if hit occured. xyz components contain UVW barycentric coordinates. w component contains hit distance.
     * @param k Maximum dimensions, get this from calculateRayDimensions().
     * @param s Shear constraints, get this from calculateRayShearConstraints().
     * @details These last two parameters are calculated once per ray(not per triangle).
     * @return True if intersection occurs.
     */
    bool rayTriangleIntersection(const Eigen::Vector3d& a, const Eigen::Vector3d& b, const Eigen::Vector3d& c, const Eigen::Vector3d& origin, Eigen::Vector4d& hit_info,
                                 const Eigen::Matrix<uint8_t,3,1>& k, const Eigen::Vector3d& s){
        //Vertices relative to origin
        Eigen::Vector3d a_local = a - origin;
        Eigen::Vector3d b_local = b - origin;
        Eigen::Vector3d c_local = c - origin;

        //Shear and scale vertices
        double ax = a_local[k.x()] - s.x()*a_local[k.z()];
        double ay = a_local[k.y()] - s.y()*a_local[k.z()];
        double bx = b_local[k.x()] - s.x()*b_local[k.z()];
        double by = b_local[k.y()] - s.y()*b_local[k.z()];
        double cx = c_local[k.x()] - s.x()*c_local[k.z()];
        double cy = c_local[k.y()] - s.y()*c_local[k.z()];
        //todo see if this can be converted to SIMD vector math optimization

        //scaled barycentric coordinates
        double u = cx*by-cy*bx;
        double v = ax*cy-ay*cx;
        double w = bx*ay-by*ax;

        //Edge tests
        if ((u<0.0 || v<0.0 || w<0.0) && (u>0.0 || v>0.0 || w>0.0)) return false;

        double determinant = u + v + w;

        if(determinant == 0.0) return false;

        //calculate hit distance
        double az = s.z()*a_local[k.z()];
        double bz = s.z()*b_local[k.z()];
        double cz = s.z()*c_local[k.z()];
        double t = u*az + v*bz + w*cz;
        //todo see if this can become dot product

        //depth test. The std::copysign() is used to get either 1.0 or -1.0.
        if(t * std::copysign(1.0,determinant) < 0.0) return false;

        //normalize
        hit_info = {u,v,w,t};
        hit_info *= 1.0/determinant;
        return true;
    }

    //todo impl https://research.nvidia.com/sites/default/files/pubs/2019-03_Cool-Patches%3A-A/Chapter_08.pdf
    //todo impl refining approximation that is better for GPU use: https://www.reedbeta.com/blog/quadrilateral-interpolation-part-2/
    

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
             //  if(rayTriangleIntersection(tri_mesh.vertices[tri_mesh.triangle_indices[triangle_index+0]].cast<double>(),tri_mesh.vertices[tri_mesh.triangle_indices[triangle_index+1]].cast<double>(),tri_mesh.vertices[tri_mesh.triangle_indices[triangle_index+2]].cast<double>(),
            //                              point_initial,direction,distance)){
            //       double time_of_impact = distance / speed;
            //       if(time_of_impact < earliest){
             //          earliest = time_of_impact;
                  //     earliest_point = point_initial + direction * distance;
                //   }
           //    }
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