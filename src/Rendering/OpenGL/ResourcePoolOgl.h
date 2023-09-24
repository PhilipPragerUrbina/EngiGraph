//
// Created by Philip on 9/18/2023.
//

#pragma once
#include "glad/glad.h"
#include "../../Geometry/VisualMesh.h"
#include "../../Exceptions/RuntimeException.h"
#include "../../Image/Image.h"

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
            uint32_t indices_size;
            //todo add boolean checking if deallocated and throw error if accessing deallocated mesh.
        };

        /**
         * Contains info about gpu texture.
         */
        struct OGLTexture{
            uint32_t width, height;
            khronos_uint32_t texture_id;
        };

        /**
         * Simple ID corresponding to a mesh in GPU memory.
         */
        typedef uint32_t OGLMeshID;

        /**
        * Simple ID corresponding to a texture in GPU memory.
        */
        typedef uint32_t OGLTextureID;

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
        * Load a 4 channel 32 bit texture into gpu memory.
        * @param cpu_texture Texture to load.
        * @throws RuntimeException Issue loading texture.
        * @return Texture resource id.
        */
        [[nodiscard]] OGLTextureID loadTexture(const Image<uint32_t>& cpu_image);



        /**
         * Get an openGL texture from a texture id.
         * @param id Valid texture mesh id from loadTexture().
         * @warning Asserts that id is valid.
         * @return OpenGL texture information.
         */
        [[nodiscard]] OGLTexture getTexture(OGLTextureID id) const;

        /**
         * Delete resources
         */
        ~ResourcePoolOGL();
    private:
        std::vector<OGLMesh> meshes;
        std::vector<OGLTexture> textures;
    };

} // EngiGraph
