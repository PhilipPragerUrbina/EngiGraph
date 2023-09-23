//
// Created by Philip on 9/18/2023.
//

#include "RenderContextOgl.h"

namespace EngiGraph {
    /**
    * Global flag that makes sure glad is only initialized once.
    */
    bool GLAD_INITIALIZED = false;

    RenderContextOGL::RenderContextOGL(int width, int height, GLADloadproc gll) : width(width), height(height){
        if(!GLAD_INITIALIZED){
            if (!gladLoadGLLoader(gll))
            {
                throw RuntimeException("OpenGL context : Failed to load GLAD GLL");
            }
            GLAD_INITIALIZED = true;
        }
    }

    RenderContextOGL::OGLPipelineID RenderContextOGL::addPipeLine(GeneralPipeLineOgl *pipeline) {
        pipeline->resize(width,height);
        pipelines.push_back(pipeline);
        return pipelines.size()-1;
    }

    RenderContextOGL::~RenderContextOGL() {
        for (auto pipeline : pipelines) {
            delete pipeline;
        }
    }

    void RenderContextOGL::resize(int new_width, int new_height) {
        width = new_width;
        height = new_height;
        for (auto pipeline : pipelines) {
            pipeline->resize(width,height);
        }
    }

    GeneralPipeLineOgl::FrameBuffer RenderContextOGL::renderAll() {
        //todo impl blend shader
        return GeneralPipeLineOgl::FrameBuffer();
    }

    GeneralPipeLineOgl::FrameBuffer RenderContextOGL::render(RenderContextOGL::OGLPipelineID id) {
        pipelines[id]->render(resource_pool);
        return pipelines[id]->getMainFramebuffer();
    }


} // EngiGraph