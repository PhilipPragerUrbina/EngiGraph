//
// Created by Philip on 9/18/2023.
//

#pragma once
#include <Eigen>
#include <vector>

namespace EngiGraph {

    /**
     * Contains a mesh with common attributes for rendering.
     * @details Vertex position, UV coordinate, and normals.
     */
    struct VisualMesh {

        /**
         * Name provided for mesh. Not guaranteed to be unique.
         */
        std::string name;

        /**
         * Single vertex structure for sequential memory layout.
         */
        struct Vertex{
            /**
            * Local space position.
            */
            Eigen::Vector3f position;

            /**
             * Local space normal, expected to be unit vector.
             */
            Eigen::Vector3f normal;

            /**
             * UV coordinates representing texture coordinates.
             * @warning Not guaranteed to be in a specific range, wrapping is expected.
             */
             Eigen::Vector2f uv_coordinate;

        };

        /**
         * Each vertex represents a point in the mesh.
         */
        std::vector<Vertex> vertices{};

        /**
         * Every n indices make up one face, usually 3 make up a triangle.
         */
        std::vector<uint32_t> indices{};

        //todo index combining function
        //todo check if any vertices are referenced by indices that are out of bounds
        //todo check if any vertices can be removed since they are not referenced by indices.

        /**
         * Check if the mesh is a valid triangular mesh.
         * @warning This does not strictly tell you if the mesh is 100% triangular(since it might have 6 sided faces), just that it can be rendered as a triangular mesh.
         * @return True if indices are evenly divisible by 3.
         */
        [[nodiscard]] bool isTriangular() const {
            return indices.size() % 3 == 0;
        }

        /**
         * Get the number of triangles(3 index faces) in the mesh.
         */
        [[nodiscard]] size_t getTriangleCount() const {
            return indices.size() / 3;
        }
    };

} // EngiGraph
