//
// Created by Philip on 9/28/2023.
//

#pragma once

#include "../Resources/TextureResourceOgl.h"
#include "../../../Image/Image.h"
#include <memory>

namespace EngiGraph {

    /**
     * Load a 32 bit RGBA image onto the GPU as an OpenGl texture.
     * @details Uses standard texture options such as linear mipmaps and wrapping.
     * @param cpu_image Texture data on cpu.
     * @return A pointer to a gpu resource that will be deallocated when the pointer is destroyed.
     */
        std::shared_ptr<TextureResourceOgl> loadTextureOgl(const Image<uint32_t>& cpu_image);

} // EngiGraph
