//
// Created by Philip on 9/26/2023.
//

#include "MeshResourceOgl.h"

namespace EngiGraph {

    MeshResourceOgl::~MeshResourceOgl() {
        glDeleteBuffers(1,&element_buffer_id);
        glDeleteBuffers(1,&vertex_buffer_id);
        glDeleteVertexArrays(1,&vertex_array_id);
    }
} // EngiGraph