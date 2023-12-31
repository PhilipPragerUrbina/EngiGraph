//
// Created by Philip on 10/4/2023.
//

#include "LinearPointCcd.h"
#include <vector>
#include <iostream>
#include <optional>

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

        //Shear and scale vertices so that this is all in 'ray space'
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

        //Edge tests. Is it within the triangle.
        if ((u<0.0 || v<0.0 || w<0.0) && (u>0.0 || v>0.0 || w>0.0)) return false;

        double determinant = u + v + w;

        if(determinant == 0.0) return false; //Parallel is considered to not be a hit.

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

    /**
     * Linearly interpolate between values.
     * @param a Start when t = 0.
     * @param b End when t = 1.
     * @param t Value.
     * @return Lerped vector.
     */
    Eigen::Vector3d lerp(const Eigen::Vector3d& a, const Eigen::Vector3d& b, const double& t) {
        return a + (b-a) * t;
    }

/**
     * Intersect a ray with a bi-linear quad patch.
     * @details This does not perform backface culling.
     * The quad can be planar or non-planar.
     * @see https://research.nvidia.com/sites/default/files/pubs/2019-03_Cool-Patches%3A-A/Chapter_08.pdf
     * @param q00,q01 Vertex positions of first edge.
     * @details First edge to second edge increases u coordinate. q00 to q10 and q01 to q11 increases v coordinate.
     * @param q10,q11 Vertex positions of second edge.
     * @param origin Origin of ray.
     * @param direction Unit vector direction of ray.
     * @param hit_info Output (u,v,distance) if hit occurs.
     * @param max_distance Max distance the hit can be away before hit in not registered.
     * @return True if hit occurred.
     */
    bool rayQuadPatchIntersection(Eigen::Vector3d q00, const Eigen::Vector3d& q01, Eigen::Vector3d q10, const Eigen::Vector3d& q11, const Eigen::Vector3d& origin, const Eigen::Vector3d& direction, Eigen::Vector3d& hit_info,
                                  const double& max_distance){

        Eigen::Vector3d e10 = q10 - q00;
        Eigen::Vector3d e11 = q11 - q10;
        Eigen::Vector3d e00 = q01 - q00;
        Eigen::Vector3d qn = (q10 - q00).cross(q01-q11); //todo precompute these values

        q00 -= origin;
        q10 -= origin;
        //Quadratic formula coefficients
        double a = q00.cross(direction).dot(e00);
        double c = qn.dot(direction);
        double b = q10.cross(direction).dot(e11) - (a + c);

        double determinant = b*b - 4*a*c;
        if(determinant < 0.0) return false; //Facing away
        determinant = sqrt(determinant);

        double u1, u2; //roots
        double t = max_distance;
        double u,v;

        if(c == 0.0){ //Only one root
            u1 = -a/b; u2 = -1;
        }else{ //Two roots, computing the stable one first, then using viete's formula to get the second.
            u1 = (-b - std::copysign(determinant,b))/2.0;
            u2 = a/u1;
            u1 /= c;
        }
        //Which root is inside the patch?
        if(0.0 <= u1 && u1 <= 1.0) {
            Eigen::Vector3d pa = lerp(q00, q10, u1);
            Eigen::Vector3d pb = lerp(e00, e11, u1);
            Eigen::Vector3d n = direction.cross(pb);
            determinant = n.dot(n);
            n = n.cross(pa);
            double t1 = n.dot(pb);
            double v1 = n.dot(direction);
            if (t1 > 0.0 && 0.0 <= v1 && v1 <= determinant) {
                t = t1 / determinant;
                u = u1;
                v = v1 / determinant;
            }
        }
        if (0.0 <= u2 && u2 <= 1.0) {
            Eigen::Vector3d pa = lerp(q00, q10, u2);
            Eigen::Vector3d pb = lerp(e00, e11, u2);
            Eigen::Vector3d n = direction.cross(pb);
            determinant = n.dot(n);
            n = n.cross(pa);
            double t2 = n.dot(pb)/determinant;
            double v2 = n.dot(direction);
            if (0.0 <= v2 && v2 <= determinant && t > t2 && t2 > 0.0) {
                t = t2; u = u2;
                v = v2/determinant;
            }
        }
        hit_info = {u,v,t};
        return t < max_distance;
    }

    /**
     * Get the flat normal of a triangle.
     * @param a,b,c Vertices.
     * @return Surface normal.
     */
    Eigen::Vector3d getNormal(const Eigen::Vector3d& a, const Eigen::Vector3d& b, const Eigen::Vector3d& c){
        //todo precompute
        return  (b - a).cross(a - c).normalized();
    }

    /**
     * Get the normal between two edges(lines).
     * @param a_1 Edge 1 point 1.
     * @param a_2 Edge 1 point 2.
     * @param b_1 Edge 2 point 1.
     * @param b_2 Edge 2 point 2.
     * @warning If parallel, the positions of the edges will be used. If equal lines, some kind of normal will be returned.
     * @return The contact normal between the two edges. Polarity is not guaranteed.
     */
    Eigen::Vector3d getNormalEdgeToEdge(const Eigen::Vector3d& a_1, const Eigen::Vector3d& a_2,const Eigen::Vector3d& b_1,const Eigen::Vector3d& b_2){
        auto normal_a = (a_1-a_2).normalized();
        auto normal_b = (b_1-b_2).normalized();
        if(abs(normal_a.dot(normal_b)) > 0.999 ) { //Parallel
            auto projected_point_a = a_1 - normal_a*a_1.dot(normal_a);
            auto projected_point_b = b_1 - normal_b*b_1.dot(normal_b);
            if(projected_point_a == projected_point_b) return {1,0,0}; //todo remove as it should never happen
            return (projected_point_b-projected_point_a).normalized();
        }
        return normal_a.cross(normal_b);
    }

    //todo benchmark single vs double precision

    /**
     * Do linear CCD treating one mesh as moving, and the other as stationary.
     * @param tri_mesh Stationary triangle mesh.
     * @param point_mesh Moving 'point' mesh.
     * @param point_mesh_initial Start transform of point mesh.
     * @param point_mesh_final Target transform of point mesh.
     * @param check_edges Weather or not to do edge to edge detection as well.
     * @param time_delta Max difference between times such that they are considered simultaneous.
     * @details To check two moving objects, make the motion of one relative to the other.
     * @details To get all collisions between two objects, one must simply run this twice, once with mesh A as points, and once with mesh B as points. Edges only need to be checked once.
     * @return All hits.
     * @warning Assumes that final and initial positions of mesh are not the same.
     */
    std::vector<CCDHit> linearCCDOneWay(const Mesh &tri_mesh, const Mesh &point_mesh, const Eigen::Matrix4d &point_mesh_initial,  const Eigen::Matrix4d &point_mesh_final, bool check_edges, double time_delta){
        std::vector<CCDHit> hits;
        double earliest_time = 1.0;

        //Point to face CCD
        for (const auto& local_point : point_mesh.vertices) {

            //Transform points to initial and final positions
            auto local_point_4 = Eigen::Vector4d (local_point.x(),local_point.y(),local_point.z(),1.0f);
            Eigen::Vector3d point_initial = (point_mesh_initial * local_point_4).head<3>();
            Eigen::Vector3d point_final = (point_mesh_final * local_point_4).head<3>();

            //Calculate ray information
            Eigen::Vector3d point_difference = point_final - point_initial;
            double point_distance = point_difference.norm();
            Eigen::Vector3d point_direction = point_difference.normalized();
            auto k = calculateRayDimensions(point_direction);
            auto s = calculateRayShearConstraints(k,point_direction);

            //Trace ray along path of vertex
            Eigen::Vector4d hit_info{};
            for (int triangle_index = 0; triangle_index < tri_mesh.triangle_indices.size(); triangle_index += 3) {
               if(rayTriangleIntersection(tri_mesh.vertices[tri_mesh.triangle_indices[triangle_index+0]].cast<double>(),tri_mesh.vertices[tri_mesh.triangle_indices[triangle_index+1]].cast<double>(),tri_mesh.vertices[tri_mesh.triangle_indices[triangle_index+2]].cast<double>(),
                       point_initial,hit_info,k,s)){
                   double time = hit_info.w() / point_distance;
                   if(time < earliest_time + time_delta){
                       if(time < earliest_time - time_delta) {
                           hits.clear();
                           earliest_time = time;
                       }
                       CCDHit hit{};
                       hit.time = time;
                       hit.global_point = hit_info.w() * point_direction + point_initial; //local point for now
                       hit.normal_a_to_b = getNormal(tri_mesh.vertices[tri_mesh.triangle_indices[triangle_index+0]].cast<double>(),tri_mesh.vertices[tri_mesh.triangle_indices[triangle_index+1]].cast<double>(),tri_mesh.vertices[tri_mesh.triangle_indices[triangle_index+2]].cast<double>());
                       hit.normal_a_to_b *= hit.normal_a_to_b.dot(point_direction) > 0.0 ? -1.0 : 1.0; //Allow backfaces to have correct normal as well.
                       hits.push_back(hit);
                   }
               }
            }
        }

        if(!check_edges) return hits;

        //Edge to edge CCD
        //todo allow smooth normals
        for (int edge_stay = 0; edge_stay < tri_mesh.edge_indices.size(); edge_stay += 2) {
            for (int edge_move = 0; edge_move < point_mesh.edge_indices.size(); edge_move += 2) {
                    //todo re-use transformations from before

                    //Ray is traced along the stationary edge
                    Eigen::Vector3d stay_a = tri_mesh.vertices[tri_mesh.edge_indices[edge_stay+0]].cast<double>();
                    Eigen::Vector3d stay_b = tri_mesh.vertices[tri_mesh.edge_indices[edge_stay+1]].cast<double>();
                    double stay_edge_length = (stay_a-stay_b).norm();
                    Eigen::Vector3d stay_direction = (stay_b-stay_a).normalized();

                    //moving edge becomes a quad
                    Eigen::Vector3d move_a_local = point_mesh.vertices[point_mesh.edge_indices[edge_move+0]].cast<double>();
                    Eigen::Vector3d move_b_local = point_mesh.vertices[point_mesh.edge_indices[edge_move+1]].cast<double>();

                    Eigen::Vector3d move_a_init = (point_mesh_initial * Eigen::Vector4d (move_a_local.x(),move_a_local.y(),move_a_local.z(),1.0f)).head<3>();
                    Eigen::Vector3d move_b_init = (point_mesh_initial * Eigen::Vector4d (move_b_local.x(),move_b_local.y(),move_b_local.z(),1.0f)).head<3>();
                    Eigen::Vector3d move_a_final = (point_mesh_final * Eigen::Vector4d (move_a_local.x(),move_a_local.y(),move_a_local.z(),1.0f)).head<3>();
                    Eigen::Vector3d move_b_final = (point_mesh_final * Eigen::Vector4d (move_b_local.x(),move_b_local.y(),move_b_local.z(),1.0f)).head<3>();

                    Eigen::Vector3d hit_info{};
                    if(rayQuadPatchIntersection(move_a_init,move_b_init, move_a_final,move_b_final, stay_a,stay_direction,hit_info,stay_edge_length)){
                        double time = hit_info.x(); //u coordinate
                        if(time < earliest_time + time_delta) {
                            if (time < earliest_time - time_delta) {
                                hits.clear();
                                earliest_time = time;
                            }
                            CCDHit hit{};
                            hit.time = time;
                            hit.global_point = stay_a + stay_direction * hit_info.z();
                            //todo check
                            const double normal_rollback = 0.0001; //slight time offset to prevent equal edges.
                            hit.normal_a_to_b = getNormalEdgeToEdge(stay_a, stay_b, lerp(move_a_init, move_a_final,
                                                                                         time - normal_rollback),
                                                                    lerp(move_b_init, move_b_final,
                                                                         time - normal_rollback));
                            hit.normal_a_to_b *= hit.normal_a_to_b.dot(move_a_final - move_a_init) > 0.0 ? -1.0
                                                                                                         : 1.0; //Allow backfaces to have correct normal as well. In this case we use a point on the original edge to estimate the correct direction.
                            hits.push_back(hit);
                        }
                    }
            }
        }

        return hits;
    }

    //todo doc
    std::vector<CCDHit> combineClosePoints(const std::vector<CCDHit>& original_hits, double delta){
        std::vector<CCDHit> filtered_hits{};
        std::vector<int> counts{}; //counts for averages
        for (const auto& hit_original : original_hits) {
            bool found = false;
            for (auto& hit_filtered : filtered_hits) {
                if((hit_filtered.global_point - hit_original.global_point).norm() < delta){
                    //average the normal
                    hit_filtered.normal_a_to_b += hit_original.normal_a_to_b;
                    found = true; //todo ugly
                    break;
                }
            }
            if(!found){
                filtered_hits.push_back(hit_original);
                counts.push_back(1);
            }
        }
        //todo check if ther is a  better way to average normals
        for (int j = 0; j < filtered_hits.size(); ++j) {
            filtered_hits[j].normal_a_to_b /= (double)counts[j];
            filtered_hits[j].normal_a_to_b.normalize();
        }
        return filtered_hits;
    }

   std::vector<CCDHit> linearCCD(const Mesh &a, const Mesh &b, const Eigen::Matrix4d &a_initial, const Eigen::Matrix4d &b_initial,
              const Eigen::Matrix4d &a_final, const Eigen::Matrix4d &b_final) {
        //todo determine time delta and normal rollback from amount of movement
       const double time_delta = 0.00001;
       //todo determine this from mesh size and detail
       const double collision_point_combine_delta = 0.0001;


        if(a_initial.isApprox(a_final) && b_initial.isApprox( b_final)) return {}; //no movement

        //Go both directions
        std::vector<CCDHit> b_rel_to_a = linearCCDOneWay(a,b, a_initial.inverse() * b_initial,a_final.inverse()*b_final,true, time_delta);
        std::vector<CCDHit> a_rel_to_b = linearCCDOneWay(b,a, b_initial.inverse() * a_initial,b_final.inverse()*a_final,false, time_delta);

       //convert to global space
       for (auto& hit : b_rel_to_a) {
           hit.global_point = lerp((a_initial * Eigen::Vector4d(hit.global_point.x(),hit.global_point.y(),hit.global_point.z(),1.0)).head<3>(),(a_final * Eigen::Vector4d(hit.global_point.x(),hit.global_point.y(),hit.global_point.z(),1.0)).head<3>(),hit.time);
           //todo optimize and check for correct lerp
           //This coule cause problems if an object turns 180 degrees. (0,0,1) + (0,0,-1) = (0,0,0) at t = 0.5
           hit.normal_a_to_b = lerp((a_initial.inverse().transpose()).topLeftCorner<3,3>() * hit.normal_a_to_b,(a_final.inverse().transpose()).topLeftCorner<3,3>() * hit.normal_a_to_b, hit.time);
       }
       for (auto& hit : a_rel_to_b) {
           hit.global_point = lerp((b_initial * Eigen::Vector4d(hit.global_point.x(),hit.global_point.y(),hit.global_point.z(),1.0)).head<3>(),(b_final * Eigen::Vector4d(hit.global_point.x(),hit.global_point.y(),hit.global_point.z(),1.0)).head<3>(),hit.time);
           hit.normal_a_to_b *= -1.0; //flip such that normal is a to b
           hit.normal_a_to_b = lerp((b_initial.inverse().transpose()).topLeftCorner<3,3>() * hit.normal_a_to_b,(b_final.inverse().transpose()).topLeftCorner<3,3>() * hit.normal_a_to_b, hit.time);
       }

       //Check for earliest time
       double time_a = b_rel_to_a.empty() ? 1.0 : b_rel_to_a[0].time;
       double time_b = a_rel_to_b.empty() ? 1.0 : a_rel_to_b[0].time;

       if(abs(time_a - time_b) < time_delta){ //Both
           b_rel_to_a.insert(b_rel_to_a.end(),a_rel_to_b.begin(),a_rel_to_b.end());
           return combineClosePoints(b_rel_to_a,collision_point_combine_delta);
       }
       if(time_a < time_b){ //List a has earlier times
           return combineClosePoints(b_rel_to_a,collision_point_combine_delta);
       }
       return combineClosePoints(a_rel_to_b,collision_point_combine_delta); //b has earlier times
    }

} // EngiGraph