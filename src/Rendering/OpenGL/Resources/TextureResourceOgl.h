//
// Created by Philip on 9/26/2023.
//

#pragma once

#include "glad/glad.h"

namespace EngiGraph {

    /**
     * A "pointer" to a texture on the gpu for openGl.
     * @details Will deallocate data on gpu destruction, so it is better to use this as a pointer.
     */
    struct TextureResourceOgl {
        khronos_uint32_t width, height;
        khronos_uint32_t texture_id;

        ~TextureResourceOgl();
    };

} // EngiGraph
