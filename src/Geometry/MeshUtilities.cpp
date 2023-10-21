//
// Created by Philip on 10/4/2023.
//

#include <unordered_set>
#include "MeshUtilities.h"

namespace EngiGraph {

    Mesh reduceMesh(const Mesh &input, float combine_delta) {
        std::vector<Eigen::Vector3f> vertices;
        std::map<uint32_t,uint32_t> vertex_mappings;

        //combine vertices
        for (int j = 0; j < input.vertices.size(); ++j) {
            auto input_vertex = input.vertices[j];
            int found_id = -1;
            for (int k = 0; k < vertices.size(); ++k) {
                auto existing_vertex = vertices[k];
                if((existing_vertex-input_vertex).norm() < combine_delta){
                    found_id = k;
                    break;
                }
            }
            if(found_id == -1){
                vertex_mappings[j] = vertices.size();
                vertices.push_back(input_vertex);
            }else{
                vertex_mappings[j] = found_id;
            }
        }
        //generate new triangles
        std::vector<uint32_t> indices;
        indices.reserve(input.triangle_indices.size());

        for (auto input_index : input.triangle_indices) {
            indices.push_back(vertex_mappings[input_index]);
        }

        //Generate unique edges
        std::unordered_set<uint64_t> edge_indices_set;
        //Encode edges as 64 bit indices hash: https://stackoverflow.com/questions/6064566/concatenate-two-32bit-numbers-to-get-a-64bit-result
        //This is evil, but it works
        for (int tri_id = 0; tri_id < indices.size(); tri_id+=3) {
            auto ordering_0 = std::minmax(indices[tri_id+0],indices[tri_id+1]); //Ensure consistent ordering
            edge_indices_set.emplace(((uint64_t)ordering_0.first << 32) | ordering_0.second);
            auto ordering_1  = std::minmax(indices[tri_id+1],indices[tri_id+2]);
            edge_indices_set.emplace(((uint64_t)ordering_1.first << 32) | ordering_1.second);
            auto ordering_2  = std::minmax(indices[tri_id+2],indices[tri_id+0]);
            edge_indices_set.emplace(((uint64_t)ordering_2.first << 32) | ordering_2.second);
        }

        //convert to vector
        std::vector<uint32_t> edge_indices;
        edge_indices.reserve(edge_indices_set.size() * 2);
        for (const auto& edge : edge_indices_set) {
            edge_indices.push_back(((const uint32_t*)&edge)[0]);
            edge_indices.push_back(((const uint32_t*)&edge)[1]);
        }

        return {vertices,indices, edge_indices};
    }

} // EngiGraph