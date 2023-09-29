//
// Created by Philip on 9/23/2023.
//
#include <cassert>
#include "PipelineOgl.h"
#include "../../Exceptions/RuntimeException.h"
namespace EngiGraph {

    bool GLAD_INITIALIZED = false;

    void initOpenGl(GLADloadproc gll) {
        if(!GLAD_INITIALIZED){
            if (!gladLoadGLLoader(gll))
            {
                throw RuntimeException("OpenGL context : Failed to load GLAD GLL");
            }
            GLAD_INITIALIZED = true;
        }
    }


    PipelineOgl::PipelineOgl(int width, int height) {
        resize(width,height);
    }

    PipelineOgl::~PipelineOgl() {
        glDeleteRenderbuffers(1,&main_framebuffer.render_buffer);
        glDeleteTextures(1,&main_framebuffer.color_texture);
        glDeleteFramebuffers(1,&main_framebuffer.frame_buffer);
    }

    void PipelineOgl::resize(int width, int height) {

        if(main_framebuffer.initialized){
            glDeleteRenderbuffers(1,&main_framebuffer.render_buffer);
            glDeleteTextures(1,&main_framebuffer.color_texture);
            glDeleteFramebuffers(1,&main_framebuffer.frame_buffer);
        }

        main_framebuffer.initialized = true;
        main_framebuffer.width = width;
        main_framebuffer.height = height;

        glGenFramebuffers(1, &main_framebuffer.frame_buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, main_framebuffer.frame_buffer);

        glGenTextures(1, &main_framebuffer.color_texture);
        glBindTexture(GL_TEXTURE_2D, main_framebuffer.color_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, main_framebuffer.color_texture, 0);

        glGenRenderbuffers(1, &main_framebuffer.render_buffer);
        glBindRenderbuffer(GL_RENDERBUFFER, main_framebuffer.render_buffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, main_framebuffer.render_buffer);

        assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        resizeCallBack(width,height);
    }
}