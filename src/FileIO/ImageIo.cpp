//
// Created by Philip on 9/11/2023.
//

#include "ImageIo.h"
#include <stdexcept>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace EngiGraph {

    //Reads image into 8 bit 4 channel format using stb image.
    Image<uint32_t> readImage(const std::string& file){
        throw std::invalid_argument("File not found");
    }
    //todo finish

} // EngiGraph