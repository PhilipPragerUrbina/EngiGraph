//
// Created by Philip on 9/11/2023.
//

#pragma once

#include "../Image/Image.h"
#include <cstdint>
#include <string>

namespace EngiGraph {

    /**
     * Read an RGBA image off of the filesystem.
     * @param file Path to image file.
     * @note Supported formats: PNG, JPG/JPEG, TGA, BMP.
     * @return 8 bit RGBA image.
     * @throws RuntimeException if unable to open the file.
     */
    Image<uint32_t> readImage(const std::string& file);

    // todo image write

} // EngiGraph
