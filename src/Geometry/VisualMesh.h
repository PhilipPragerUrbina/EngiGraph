//
// Created by Philip on 9/18/2023.
//

#pragma once
#include <Eigen>
#include <vector>

namespace EngiGraph {

    /**
     * Contains a triangular mesh with common attributes for rendering.
     * @details Vertex position, UV coordinate, and normals.
     */
    struct VisualMesh {

        /**
         * Name provided for mesh. Not guaranteed to be unique.
         */
        std::string name;

        /**
         * Local space positions of vertices.
         */
        std::vector<Eigen::Vector3f> positions{};
        /**
         * Vertex normals, expected to be normalized.
         */
        std::vector<Eigen::Vector3f> normals{};
        /**
         * UV coordinates representing texture coordinates.
         * @warning Not guaranteed to be in a specific range, wrapping is expected.
         */
        std::vector<Eigen::Vector2f> uv_coordinates{};

        /**
         * Every 3 indices make up one triangle.
         */
        std::vector<uint32_t> indices{};

        /**
         * Check if the mesh is a valid triangular mesh.
         * All vertex attribute arrays must be the same size, and indices must form triangles.
         * @return True if valid.
         */
        [[nodiscard]] bool validate() const {
            return positions.size() == normals.size() && normals.size() == uv_coordinates.size() && indices.size() % 3 == 0;
        }

        /**
         * Get the number of triangles(faces) in the mesh.
         */
        [[nodiscard]] size_t getTriangleCount() const {
            return indices.size() / 3;
        }
    };

} // EngiGraph
