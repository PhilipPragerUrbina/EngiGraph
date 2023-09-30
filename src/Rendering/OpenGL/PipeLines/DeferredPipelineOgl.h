//
// Created by Philip on 9/29/2023.
//

#pragma once

#include "../PipelineOgl.h"
#include "../Resources/TextureResourceOgl.h"
#include <memory>
#include <vector>
#include "../../Camera.h"
#include "../ShaderOgl.h"

namespace EngiGraph {

    /**
     * Pipeline that uses a deferred shading scheme.
     */
    class DeferredPipelineOgl : public PipelineOgl {
    public:

        /**
         * Information needed to render a mesh.
         */
        struct DrawCall{
            /**
             * Mesh to render in local space.
             */
            std::shared_ptr<MeshResourceOgl> mesh;
            /**
             * Material albedo texture(rgba).
             */
            std::shared_ptr<TextureResourceOgl> albedo;
            /**
             * Object transform matrix.
             */
            Eigen::Matrix4f transform;
        };

        /**
         * Main camera
         */
        Camera camera;

        /**
         * Ambient lighting color.
         * @details Background color that is also added to object color.
         * May be in any range, but usually between 0.0f to 1.0f.
         */
        Eigen::Vector3f ambient_color = {0.0f,0.0f,0.0f};

        /**
         * Submit a draw call to be rendered on the next render().
         * @param draw_call Draw call information.
         */
        void submitDrawCall(const DrawCall& draw_call);

        /**
         * Create a deferred shading pipeline.
         * @param camera Camera to start with.
         */
        DeferredPipelineOgl(int width, int height, const Camera& camera) : camera(camera) , PipelineOgl(width,height) {
            glGenVertexArrays(1, &empty_vao);
        }

        /**
         * Destroy internal resources.
         */
        ~DeferredPipelineOgl() override;

        void render() override;

    protected:

        void resizeCallBack(int width, int height) override;

    private:

        khronos_uint32_t empty_vao; //Used to render full screen

        khronos_uint32_t g_buffer; // framebuffer that contains pre shading information
        bool g_buffer_initialized = false; //Does the framebuffer exist yet?
        khronos_uint32_t g_position, g_normal, g_albedo, g_depth; //Framebuffer textures(depth is a render buffer).

        //First shading pass
        ShaderOGL shader_g_pass{"./shaders/deferred_vertex_g.glsl", "./shaders/deferred_fragment_g.glsl"};
        //Second shading pass
        ShaderOGL shader_shading_pass{"./shaders/deferred_vertex_shading.glsl", "./shaders/deferred_fragment_shading.glsl"};

        std::vector<DrawCall> draw_calls{};
    };

} // EngiGraph
