//
// Created by Philip on 9/23/2023.
//

#pragma once
#include "ResourcePoolOgl.h"
namespace EngiGraph {
    /**
     * Template-less parent of PipeLine OGL.
     * @see PipeLineOgl
     */
    class GeneralPipeLineOgl {
    public:
        /**
         * Contains frame buffer data on gpu.
         */
        struct FrameBuffer{
            khronos_uint32_t frame_buffer;
            khronos_uint32_t color_texture;
            khronos_uint32_t render_buffer;
            int width, height;
            bool initialized = false;
        };

        /**
         * Delete the main framebuffer.
         */
        virtual ~GeneralPipeLineOgl();

        /**
         * Render all the draw calls using whatever method.
         * @param resource_pool Contains gpu resources such as textures, meshes, and other data that is shared between pipelines in a context.
         * It is expected that the output is stored in the main pipeline frame buffer.
         */
        virtual void render(const ResourcePoolOGL& resource_pool) = 0;


        /**
         * Get the main pipeline frame buffer.
         * @usage This can be used by the context to get the result of a render.
         * @usage This can be used internally to write the result of a render.
         * @warning This framebuffer is automatically created and destroyed.
         * @warning This framebuffer is automatically resized to the correct dimensions by the context.
         * @warning Asserts that the framebuffer has been initialized. If it is not, make sure to resize the pipeline before using it.
         * @return Main pipeline frame buffer.
         */
        [[nodiscard]] FrameBuffer getMainFramebuffer() const {
            assert(main_framebuffer.initialized);
            return main_framebuffer;
        }


        /**
         * Resize the pipeline to target a new render resolution.
         * Must be called before rendering starts.
         * @warning Asserts proper framebuffer creation.
         * @param width,height New dimensions in pixels.
         */
        void resize(int width, int height);

    protected:
        /**
         * This is called after main framebuffer is resized so one can update internal frame buffers or camera aspect ratios if needed.
         * This is also called when pipeline is added to a context(basically pipeline creation).
         * Use this instead of constructor if you need to initialize something with screen dimensions.
         * @param width,height New dimensions in pixels.
         */
        virtual void resizeCallBack(int width, int height) {  }
    private:
        FrameBuffer main_framebuffer{};
    };
}

