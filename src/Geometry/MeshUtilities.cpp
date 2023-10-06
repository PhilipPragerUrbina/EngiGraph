//
// Created by Philip on 10/4/2023.
//

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
        return {vertices,indices};
    }

} // EngiGraph