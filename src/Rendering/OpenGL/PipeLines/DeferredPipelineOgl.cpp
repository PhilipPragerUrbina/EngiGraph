//
// Created by Philip on 9/29/2023.
//

#include "DeferredPipelineOgl.h"

namespace EngiGraph {
    void DeferredPipelineOgl::submitDrawCall(const DeferredPipelineOgl::DrawCall &draw_call) {
        draw_calls.push_back(draw_call);
    }

    DeferredPipelineOgl::~DeferredPipelineOgl() {
        //destroy empty vao as well.
        glDeleteVertexArrays(1,&empty_vao);
        //destroy internal framebuffers
        glDeleteRenderbuffers(1,&g_depth);
        glDeleteTextures(1,&g_albedo);
        glDeleteTextures(1,&g_normal);
        glDeleteTextures(1,&g_position);
        glDeleteFramebuffers(1,&g_buffer);
    }

    void DeferredPipelineOgl::render() {
        //Initialize viewport and depth testing.
        glViewport(0,0,getMainFramebuffer().width,getMainFramebuffer().height);
        glEnable(GL_DEPTH_TEST);
        //Bind internal framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);
        //Blank start is crucial.
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

        //bind empty vao for full screen rendering
        glBindVertexArray(empty_vao);
        //bind main output frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, getMainFramebuffer().frame_buffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f,0.0f,0.0f,1.0f); //This can be any color, it will be overridden

        //Use the shading pass
        shader_shading_pass.use();

        shader_shading_pass.setUniform("ambient_color",ambient_color);

        //Bind the buffers rendered in first pass
        glActiveTexture(GL_TEXTURE0);
        shader_shading_pass.setUniform("position_buffer",0);
        glBindTexture(GL_TEXTURE_2D, g_position);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, g_normal);
        shader_shading_pass.setUniform("normal_buffer",1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, g_albedo);
        shader_shading_pass.setUniform("albedo_buffer",2);

        //Attribute less rendering for full screen
        glDrawArrays(GL_TRIANGLES, 0,3);
        //clean up
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindVertexArray(0);
        draw_calls.clear();
    }

    void DeferredPipelineOgl::resizeCallBack(int width, int height) {

        camera.setAspectRatio((float)width/(float)height);

        if(g_buffer_initialized){ //remove old
            glDeleteRenderbuffers(1,&g_depth);
            glDeleteTextures(1,&g_albedo);
            glDeleteTextures(1,&g_normal);
            glDeleteTextures(1,&g_position);
            glDeleteFramebuffers(1,&g_buffer);
        }
        g_buffer_initialized = true;

        glGenFramebuffers(1, &g_buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);

        //position texture(float3)
        glGenTextures(1, &g_position);
        glBindTexture(GL_TEXTURE_2D, g_position);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_position, 0);
        //position texture(float3)
        glGenTextures(1, &g_normal);
        glBindTexture(GL_TEXTURE_2D, g_normal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, g_normal, 0);
        //color texture(rgba uint32_t)
        glGenTextures(1, &g_albedo);
        glBindTexture(GL_TEXTURE_2D, g_albedo);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, g_albedo, 0);

        //Attach textures
        khronos_uint32_t attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);

        //Add depth render buffer
        glGenRenderbuffers(1, &g_depth);
        glBindRenderbuffer(GL_RENDERBUFFER, g_depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, g_depth);

        assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE); //Check for completion

        glBindFramebuffer(GL_FRAMEBUFFER, 0); //unbind
    }
} // EngiGraph