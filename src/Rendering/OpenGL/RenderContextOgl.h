//
// Created by Philip on 9/18/2023.
//

#pragma once
#include "GeneralPipeLineOgl.h"
namespace EngiGraph {


    /**
     * An OpenGL renderer.
     * Multiple of these may exist at one time(not thread safe).
     * Initializes OpenGL if not already initialized.
     * Renders multiple pipelines to one framebuffer.
     * Has its own shared pipeline memory scope.
     */
    class RenderContextOGL {
    private:
        std::vector<GeneralPipeLineOgl*> pipelines;
        int width,height;
    public:
        /**
         * Identifies a specific pipeline in a specific context.
         */
        typedef uint32_t OGLPipelineID;

        //todo default primitives
        //todo interface that wraps all the public members
        ResourcePoolOGL resource_pool{};

        /**
         * Create an OpenGL renderer and init OpenGL if needed.
         * @param width, height Initial rendering dimensions in pixels.
         * @param gll From glfwGetProcAddress or equivalent.
         * @throws RuntimeException Issue initializing OpenGL.
         */
        RenderContextOGL(int width, int height, GLADloadproc gll);

        /**
         * Add a pipeline to the context.
         * @param pipeline Pointer to created pipeline. The context will take ownership.
         * @return Pipeline ID to access it later.
         */
        OGLPipelineID addPipeLine(GeneralPipeLineOgl* pipeline);

        /**
         * Access a pipeline to submit draw calls or change global settings.
         * @tparam PipeLineType Type of pipeline. Must match original pipeline type.
         * @param id Valid pipeline id.
         * @return Pointer to pipeline id, may be invalidated if pipelines are added. Ownership is still tied to this class.
         */
        template<class PipeLineType> PipeLineType* accessPipeLine(OGLPipelineID id){
            assert(id < pipelines.size());
            return (PipeLineType*)pipelines[id];
        }

        /**
         * Resize the render context output.
         * @param new_width, new_height New dimensions in pixels.
         */
        void resize(int new_width, int new_height);

        /**
         * Render all current draw calls into a frame buffer.
         * @param id Pipeline to use.
         * @return Output frame buffer(maybe invalidated next tender).
         */
        [[nodiscard]] GeneralPipeLineOgl::FrameBuffer render(OGLPipelineID id);

        /**
         * Render all pipelines into a framebuffer and depth combine.
         * @return Output frame buffer(maybe invalidated next tender).
         */
        [[nodiscard]] GeneralPipeLineOgl::FrameBuffer renderAll();

        /**
         * Delete pipelines
         */
        ~RenderContextOGL();
    };

} // EngiGraph
