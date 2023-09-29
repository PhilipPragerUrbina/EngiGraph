//
// Created by Philip on 9/28/2023.
//

#include "TextureLoaderOgl.h"

namespace EngiGraph {

    std::shared_ptr<TextureResourceOgl> loadTexture(const Image<uint32_t> &cpu_image) {
        auto gpu_texture = std::make_shared<TextureResourceOgl>();

        glGenTextures(1, &gpu_texture->texture_id);
        glBindTexture(GL_TEXTURE_2D, gpu_texture->texture_id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        gpu_texture->width = cpu_image.getWidth();
        gpu_texture->height = cpu_image.getHeight();

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)gpu_texture->width, (int)gpu_texture->height, 0, GL_RGB, GL_UNSIGNED_BYTE, cpu_image.getData().data());

        glBindTexture(GL_TEXTURE_2D, 0);
    }

} // EngiGraph