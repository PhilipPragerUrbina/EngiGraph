//
// Created by Philip on 9/28/2023.
//

#include "MeshLoaderOgl.h"
#include "./src/Exceptions/RuntimeException.h"

namespace EngiGraph {

    std::shared_ptr<MeshResourceOgl> loadMeshOgl(const VisualMesh &cpu_mesh) {
        auto gpu_mesh = std::make_shared<MeshResourceOgl>();

        //make sure mesh is render-able as a triangle mesh.
        if(!cpu_mesh.isTriangular()){
            throw RuntimeException("Error loading mesh into OpenGL : " + cpu_mesh.name + " : not triangular");
        }

        glGenVertexArrays(1, &gpu_mesh->vertex_array_id);
        glGenBuffers(1, &gpu_mesh->vertex_buffer_id);
        glGenBuffers(1, &gpu_mesh->element_buffer_id);

        glBindVertexArray(gpu_mesh->vertex_array_id);
        glBindBuffer(GL_ARRAY_BUFFER, gpu_mesh->vertex_buffer_id);
        glBufferData(GL_ARRAY_BUFFER, cpu_mesh.vertices.size() * sizeof(VisualMesh::Vertex), &cpu_mesh.vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpu_mesh->element_buffer_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, cpu_mesh.indices.size() * sizeof(uint32_t),&cpu_mesh.indices[0], GL_STATIC_DRAW);

        //Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VisualMesh::Vertex), (void*)offsetof(VisualMesh::Vertex, position));
        //Normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VisualMesh::Vertex), (void*)offsetof(VisualMesh::Vertex, normal));
        //UV
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VisualMesh::Vertex), (void*)offsetof(VisualMesh::Vertex, uv_coordinate));

        glBindVertexArray(0); //unbind
        gpu_mesh->indices_size = cpu_mesh.indices.size();

        return gpu_mesh;
    }

} // EngiGraph