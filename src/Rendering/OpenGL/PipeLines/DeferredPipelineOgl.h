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
         * A point light.
         */
        struct PointLight {
            /**
             * World space position.
             */
            Eigen::Vector3f position;
            /**
             * Luminance RGB value.
             */
            Eigen::Vector3f color;

            /**
             * Multiplies the color before being sent into the shader.
             * One can also just multiply the color value directly, this is just for editing convenience,
             * as most color pickers only allow a specific range.
             */
            float brightness = 1.0f;

            /**
             * Constant light attention term.
             */
            float constant_attenuation = 1.0;
            /**
             * Linear light attention term.
             */
            float linear_attenuation = 0.7;
            /**
            * Quadratic light attention term.
            */
            float quadratic_attenuation = 1.8;
        };

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
         * Edit scene lights here.
         */
        std::vector<PointLight> point_lights{};

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
         * @warning Requires ./meshes/unit_sphere.obj mesh data to be available, or will throw an exception.
         * @warning Requires ./textures/blue_noise.png mesh data to be available, or will throw an exception.
         * @param camera Camera to start with.
         */
        DeferredPipelineOgl(int width, int height, const Camera& camera);

        /**
         * Destroy internal resources.
         */
        ~DeferredPipelineOgl() override;

        void render() override;

    protected:

        void resizeCallBack(int width, int height) override;

    private:

        std::shared_ptr<TextureResourceOgl> blue_noise;

        std::shared_ptr<MeshResourceOgl> unit_sphere; //sphere with radius of 1

        khronos_uint32_t empty_vao; //Used to render full screen

        khronos_uint32_t g_buffer; // framebuffer that contains pre shading information
        bool g_buffer_initialized = false; //Does the framebuffer exist yet?
        khronos_uint32_t g_position, g_normal, g_albedo, g_depth; //Framebuffer textures(depth is a render buffer).

        //First shading pass(Render individual attributes)
        ShaderOGL shader_g_pass{"./shaders/deferred_vertex_g.glsl", "./shaders/deferred_fragment_g.glsl"};
        //Second shading pass(Render ambient lighting)
        ShaderOGL shader_ambient_pass{"./shaders/deferred_vertex_ambient.glsl", "./shaders/deferred_fragment_ambient.glsl"};
        //Third shading pass(Render specific lights)
        ShaderOGL shader_light_pass{"./shaders/deferred_vertex_light.glsl", "./shaders/deferred_fragment_light.glsl"};
        std::vector<DrawCall> draw_calls{};
    };

} // EngiGraph
