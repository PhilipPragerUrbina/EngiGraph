//
// Created by Philip on 9/18/2023.
//

#pragma once
#include "glad/glad.h"
#include "../../Geometry/VisualMesh.h"
#include "../../Exceptions/RuntimeException.h"
namespace EngiGraph {

    /**
     * Keeps track and loads OpenGL gpu resources such as textures as meshes.
     */
    class ResourcePoolOGL {
    public:
        /**
         * Contains OpenGL mesh information
         */
        struct OGLMesh{
            khronos_uint32_t vertex_array_id, vertex_buffer_id, element_buffer_id;
            //todo add boolean checking if deallocated and throw error if accessing deallocated mesh.
        };

        /**
         * Simple ID corresponding to a mesh in GPU memory.
         */
        typedef uint32_t OGLMeshID;

        /**
         * Load a visual mesh into gpu memory.
         * @param cpu_mesh Mesh to load.
         * @throws RuntimeException Issue loading mesh.
         * @return GPU resource id.
         */
        [[nodiscard]] OGLMeshID loadMesh(const VisualMesh& cpu_mesh);

        /**
         * Get an openGL mesh from a mesh id.
         * @param id Valid openGL mesh id from loadMesh().
         * @warning Asserts that id is valid.
         * @return OpenGL mesh information.
         */
        [[nodiscard]] OGLMesh getMesh(OGLMeshID id) const;

        /**
         * Delete resources
         */
        ~ResourcePoolOGL();
    private:
        std::vector<OGLMesh> meshes;
    };

} // EngiGraph
