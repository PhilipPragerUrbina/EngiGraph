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

    class DeferredPipelineOgl : public PipelineOgl {
    public:

        struct DrawCall{
            std::shared_ptr<MeshResourceOgl> mesh;
            std::shared_ptr<TextureResourceOgl> albedo;
            Eigen::Matrix4f transform;
        };

        Camera camera;

        void submitDrawCall(const DrawCall& draw_call){
            draw_calls.push_back(draw_call);
        }

        khronos_uint32_t empty_vao;

        DeferredPipelineOgl(int width, int height, const Camera& camera) : camera(camera) , PipelineOgl(width,height) {
            glGenVertexArrays(1, &empty_vao);
        }

        void render() override {
            glViewport(0,0,getMainFramebuffer().width,getMainFramebuffer().height);
            glEnable(GL_DEPTH_TEST);

            glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);

            glClearColor(0.0f,0.0f,0.0f, 1.0f);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shader_g_pass.use();

            shader_g_pass.setUniform("view",camera.getViewMatrix());
            shader_g_pass.setUniform("projection",camera.getProjectionMatrix());

            shader_g_pass.setUniform("texture_albedo",0);
            for (const auto& call : draw_calls) {
                shader_g_pass.setUniform("model", call.transform);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, call.albedo->texture_id);
                glBindVertexArray(call.mesh->vertex_array_id);
                glDrawElements(GL_TRIANGLES, call.mesh->indices_size, GL_UNSIGNED_INT, nullptr);
            }
            glBindVertexArray(empty_vao);
            draw_calls.clear();

            glBindFramebuffer(GL_FRAMEBUFFER, getMainFramebuffer().frame_buffer);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glClearColor(0.0f,0.0f,0.0f, 1.0f);

            shader_shading_pass.use();
            glActiveTexture(GL_TEXTURE0);
            shader_shading_pass.setUniform("position_buffer",0);
            glBindTexture(GL_TEXTURE_2D, g_position);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, g_normal);
            shader_shading_pass.setUniform("normal_buffer",1);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, g_albedo);
            shader_shading_pass.setUniform("albedo_buffer",2);

            //attribute less rendering
            glDrawArrays(GL_TRIANGLES, 0,3);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glBindVertexArray(0);
        }

        //todo destructor
    protected:

        void resizeCallBack(int width, int height) override {
            camera.setAspectRatio((float)width/(float)height);

            if(g_buffer_initialized){
                glDeleteRenderbuffers(1,&g_depth);
                glDeleteTextures(1,&g_albedo);
                glDeleteTextures(1,&g_normal);
                glDeleteTextures(1,&g_position);
                glDeleteFramebuffers(1,&g_buffer);
            }
            g_buffer_initialized = true;

            glGenFramebuffers(1, &g_buffer);
            glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);

            //position texture
            glGenTextures(1, &g_position);
            glBindTexture(GL_TEXTURE_2D, g_position);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_position, 0);

// - normal color buffer
            glGenTextures(1, &g_normal);
            glBindTexture(GL_TEXTURE_2D, g_normal);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, g_normal, 0);

// - color + specular color buffer
            glGenTextures(1, &g_albedo);
            glBindTexture(GL_TEXTURE_2D, g_albedo);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, g_albedo, 0);

// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
            unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
            glDrawBuffers(3, attachments);

            glGenRenderbuffers(1, &g_depth);
            glBindRenderbuffer(GL_RENDERBUFFER, g_depth);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, g_depth);

            assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

    private:

        khronos_uint32_t g_buffer;
        bool g_buffer_initialized = false;
        khronos_uint32_t g_position, g_normal, g_albedo, g_depth;

        ShaderOGL shader_g_pass{"./shaders/deferred_vertex_g.glsl", "./shaders/deferred_fragment_g.glsl"};
        ShaderOGL shader_shading_pass{"./shaders/deferred_vertex_shading.glsl", "./shaders/deferred_fragment_shading.glsl"};

        std::vector<DrawCall> draw_calls{};
    };

} // EngiGraph
