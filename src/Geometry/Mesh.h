//
// Created by Philip on 10/4/2023.
//

#pragma once
#include <Eigen>
#include <vector>
namespace EngiGraph {

    /**
     * Triangular mesh that only contains geometric data.
     */
    struct Mesh {
        /**
         * Each unique vertex position.
         */
        std::vector<Eigen::Vector3f> vertices;

        /**
         * Indices of triangles.
         * @details Every 3 indices makes a triangle. Indices reference vertex array.
         */
        std::vector<uint32_t> triangle_indices;

    };

} // EngiGraph
