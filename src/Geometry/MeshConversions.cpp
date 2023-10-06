//
// Created by Philip on 10/4/2023.
//

#include "MeshConversions.h"
#include "MeshUtilities.h"
namespace EngiGraph {
    Mesh stripVisualMesh(const VisualMesh &input_mesh) {
        Mesh mesh;
        mesh.triangle_indices = input_mesh.indices;

        //only add position attributes
        for (const auto & vertex : input_mesh.vertices) {
            mesh.vertices.push_back(vertex.position);
        }

        return reduceMesh(mesh);
    }
} // EngiGraph