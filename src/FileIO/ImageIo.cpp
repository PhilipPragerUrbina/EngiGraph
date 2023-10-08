//
// Created by Philip on 9/11/2023.
//

#include "ImageIo.h"
#include "./src/Exceptions/RuntimeException.h"
#include "FileUtils.h"
#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "STB/stb_image_write.h"

namespace EngiGraph {

    Image<uint32_t> loadImage(const std::string &file) {
        validateFileExistence(file);
        validateFileExtensions(file,{".jpg",".jpeg",".png", ".tga",".bmp"});
        int width, height, channel_count;
        unsigned char *data = stbi_load(file.c_str(), &width, &height, &channel_count, 0);
        if(!data){
            throw RuntimeException("Problem loading image : " + file + " : " + stbi_failure_reason());
        }
        std::vector<uint32_t> image_data{};
        image_data.reserve(width*height*4);
        if(channel_count == 4){ //alpha channel already exists
            for (int i = 0; i < width*height*4; i+=4) {
                uint32_t color = *((uint32_t*)&data[i]); //Its fine trust me
                image_data.push_back(color);
            }
        } else if (channel_count == 3){ //Requires to be generated alpha channel
            for (int i = 0; i < width*height*3; i+=3) {
                uint32_t color;
                auto* color_ptr = (uint8_t*)&color;
                color_ptr[0] = data[i+0]; //r
                color_ptr[1] = data[i+1]; //g
                color_ptr[2] = data[i+2]; //b
                color_ptr[3] = 255; //a
                image_data.push_back(color);
            }
        }else{
            throw RuntimeException("Unsupported channel count : " + std::to_string(channel_count) + " : " + file);
        }
        stbi_image_free(data);
        return {(uint16_t)width,(uint16_t)height,image_data};
    }

} // EngiGraph