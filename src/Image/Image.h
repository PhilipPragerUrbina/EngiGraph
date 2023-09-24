//
// Created by Philip on 9/11/2023.
//

#pragma once

#include <cstdint>
#include <vector>
#include <cassert>

namespace EngiGraph {

    /**
     * Simple 2D image data.
     * @tparam T Pixel information.
     */
    template<typename T> class Image {
    private:
        uint16_t width, height;
        std::vector<T> data;
    public:
        /**
         * Create an image.
         * @param width,height
         * @param data Pixel data arranged in rows of x values.
         */
        Image(uint16_t width, uint16_t height, const std::vector<T>& data) : data(data), width(width), height(height) {}

        /**
         * Get the pixel width of the image.
         */
        [[nodiscard]] inline uint16_t getWidth() const {
            return width;
        }

        /**
         * Get the pixel height of the image.
         */
        [[nodiscard]] inline uint16_t getHeight() const {
            return height;
        }

        /**
         * Write to a pixel.
         * @param x,y Coordinates
         * @warning Asserts that coordinates are in bounds.
         * @param value New pixel value.
         */
        void inline writePixel(uint16_t x, uint16_t y, const T& value){
            assert(x < width);
            assert(y < height);
            data[x*width + y] = value;
        }

        /**
         * Get the internal data buffer.
         */
        const std::vector<T>& getData() const {
            return data;
        }

        /**
         * Get the pixel value at a coordinate.
         * @warning Asserts that coordinates are in bounds.
         * @param x,y Coordinates.
         * @note For best cache performance, put the x loop outside the y loop when iterating over the image.
         * @return Pixel value.
         */
        [[nodiscard]] inline T getPixel(uint16_t x, uint16_t y) const {
            assert(x < width);
            assert(y < height);
            return data[x*width + y];
        }
    };

} // EngiGraph
