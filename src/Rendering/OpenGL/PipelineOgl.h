//
// Created by Philip on 9/18/2023.
//

#pragma once
#include "GeneralPipeLineOgl.h"
namespace EngiGraph {

    /**
     * Virtual interface for OpenGL pipelines.
     * A pipeline in this context describes a rendering process.
     * A pipeline takes in draw calls containing a mesh and any additional information needed.
     * A pipeline then renders it to a framebuffer with whatever method it uses.
     * Finally, the framebuffer is given to the render context to display.
     * A pipeline can have any amount of additional methods to set global parameters, such as lights or cameras.
     * A pipeline may utilize internal frame buffers, textures, etc, as it needs, but it must manage their corresponding memory.
     * Draw call GPU memory is given through the shared-between-pipelines resource pool. (Textures, meshes, etc).
     * @tparam DrawInfo Struct that contains extra information needed to draw something. (Texture ids, material properties, transform, etc).
     */
    template <class DrawInfo> class PipelineOGL : public GeneralPipeLineOgl{
    public:
        /**
         * A request to draw a specific mesh with specific options at the next render().
         */
        struct DrawCall {
            ResourcePoolOGL::OGLMeshID mesh;
            DrawInfo attributes;
        };

        /**
         * Submit a draw call that will be rendered the next time render() is called.
         * @param draw_call Draw call specific information.
         */
        void submitDrawCall(const DrawCall& draw_call){
            draw_calls.push_back(draw_call);
        }

    protected:

        /**
         * Get the current draw calls in the call list, and clear the call list.
         */
        std::vector<DrawCall> popAllDrawCalls() {
            std::vector<DrawCall> current_calls = draw_calls;
            draw_calls.clear();
            return current_calls;
        }

    private:

        std::vector<DrawCall> draw_calls;
    };

} // EngiGraph
