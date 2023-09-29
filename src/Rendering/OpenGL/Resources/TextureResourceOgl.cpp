//
// Created by Philip on 9/26/2023.
//

#include "TextureResourceOgl.h"

namespace EngiGraph {

    TextureResourceOgl::~TextureResourceOgl() {
        glDeleteTextures(1, &texture_id);
    }
} // EngiGraph