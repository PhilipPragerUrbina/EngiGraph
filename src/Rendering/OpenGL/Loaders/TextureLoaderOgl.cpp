//
// Created by Philip on 9/28/2023.
//

#include "TextureLoaderOgl.h"

namespace EngiGraph {

    std::shared_ptr<TextureResourceOgl> loadTextureOgl(const Image<uint32_t> &cpu_image) {
        auto gpu_texture = std::make_shared<TextureResourceOgl>();

        glGenTextures(1, &gpu_texture->texture_id);
        glBindTexture(GL_TEXTURE_2D, gpu_texture->texture_id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cpu_image.getWidth(), cpu_image.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, cpu_image.getData().data());
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);

        gpu_texture->width = cpu_image.getWidth();
        gpu_texture->height = cpu_image.getHeight();

        return gpu_texture;
    }

} // EngiGraph