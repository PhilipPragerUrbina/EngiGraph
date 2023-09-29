//
// Created by Philip on 9/18/2023.
//

#pragma once
#include "Resources/MeshResourceOgl.h"
namespace EngiGraph {

    /**
     * Initialize glad.
     * @warning Must be called before using any Ogl class or one must initialize glad manually.
     * @details May be called multiple times, will only initialize the first time.
     * @param gll From glfwGetProcAddress or equivalent.
     */
    void initOpenGl(GLADloadproc gll);

    /**
     * Virtual interface for OpenGL pipelines.
     * A pipeline describes a rendering process.
     * @usage A pipeline takes in draw calls containing a mesh and any additional information needed.
     *  A pipeline then renders it to a main framebuffer with whatever method it uses.
     *  A pipeline can have any amount of additional methods to set global parameters, such as lights or cameras.
     * @details A pipeline may utilize additional internal frame buffers, but must make sure to deallocate and resize them properly.
     */
    class PipelineOgl {
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
         * Create a pipeline with initial render dimensions.
         * @param width,height Initial render dimensions in pixels.
         */
        PipelineOgl(int width, int height);

        /**
         * Delete the main framebuffer.
         */
        virtual ~PipelineOgl();

        /**
         * Render all the draw calls using whatever method.
         * It is expected that the output is stored in the main pipeline frame buffer.
         */
        virtual void render() = 0;

        /**
         * Get the main pipeline frame buffer.
         * @usage This can be used by the context to get the result of a render.
         * @usage This can be used internally to write the result of a render.
         * @warning This framebuffer is automatically created and destroyed on resize.
         * @return Main pipeline frame buffer.
         */
        [[nodiscard]] FrameBuffer getMainFramebuffer() const {
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
         * This is also called on pipeline creation.
         * Use this instead of constructor if you need to initialize something with screen dimensions.
         * @param width,height New dimensions in pixels.
         */
        virtual void resizeCallBack(int width, int height) {  }
    private:
        FrameBuffer main_framebuffer{};

    };

} // EngiGraph
