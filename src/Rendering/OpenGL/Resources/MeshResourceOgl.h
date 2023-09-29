//
// Created by Philip on 9/26/2023.
//

#pragma once

#include "glad/glad.h"

namespace EngiGraph {

    /**
     * An "pointer" to an indexed mesh on the gpu for openGl.
     * @details Will deallocate data on gpu destruction, so it is better to use this as a pointer.
     */
    struct MeshResourceOgl {
        khronos_uint32_t vertex_array_id, vertex_buffer_id, element_buffer_id;
        khronos_uint32_t indices_size;

        ~MeshResourceOgl();
    };

} // EngiGraph
