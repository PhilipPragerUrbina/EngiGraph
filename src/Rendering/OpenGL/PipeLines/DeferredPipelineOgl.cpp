//
// Created by Philip on 9/29/2023.
//

#include "DeferredPipelineOgl.h"
#include "../../../FileIO/ObjLoader.h"
#include "../Loaders/MeshLoaderOgl.h"

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
        glViewport(0,0,getMainFramebuffer().width,getMainFramebuffer().height);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);
        glClearColor(0.0f,0.0f,0.0f, 1.0f); //Blank start is crucial.
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

        //Ambient lighting pass
        shader_ambient_pass.use();
        shader_ambient_pass.setUniform("ambient_color",ambient_color);
        //Bind the buffers rendered in first pass
        glActiveTexture(GL_TEXTURE0);
        shader_ambient_pass.setUniform("position_buffer",0);
        glBindTexture(GL_TEXTURE_2D, g_position);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, g_normal);
        shader_ambient_pass.setUniform("normal_buffer",1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, g_albedo);
        shader_ambient_pass.setUniform("albedo_buffer",2);
        //Attribute less rendering for full screen
        glDrawArrays(GL_TRIANGLES, 0,3);

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ZERO);
        glBlendEquation(GL_FUNC_ADD);
        glCullFace(GL_FRONT); //Avoid shading light twice

        //Point light pass
        shader_light_pass.use();
        shader_light_pass.setUniform("screen_size", Eigen::Vector2f(getMainFramebuffer().width,getMainFramebuffer().height));
        shader_light_pass.setUniform("view",camera.getViewMatrix());
        shader_light_pass.setUniform("projection",camera.getProjectionMatrix());
        shader_light_pass.setUniform("camera_position",camera.getPosition());

        glActiveTexture(GL_TEXTURE0);
        shader_light_pass.setUniform("position_buffer",0);
        glBindTexture(GL_TEXTURE_2D, g_position);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, g_normal);
        shader_light_pass.setUniform("normal_buffer",1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, g_albedo);
        shader_light_pass.setUniform("albedo_buffer",2);

        glBindVertexArray(unit_sphere->vertex_array_id);
        //todo uniform buffer draw all lights at once with draw arrays
        for (const PointLight& light : point_lights) {
            //calculate the max radius of the light
            float light_max = std::fmaxf(std::fmaxf(light.color.x(), light.color.y()), light.color.z())*light.brightness;
            float radius =(-light.linear_attenuation +  std::sqrtf(light.linear_attenuation * light.linear_attenuation - 4.0f * light.quadratic_attenuation * (light.constant_attenuation - (256.0f / 5.0f) * light_max)))/ (2.0f * light.quadratic_attenuation);

            shader_light_pass.setUniform("light_constant", light.constant_attenuation );
            shader_light_pass.setUniform("light_linear", light.linear_attenuation);
            shader_light_pass.setUniform("light_quadratic", light.quadratic_attenuation);

            shader_light_pass.setUniform("light_scale_factor", radius );
            shader_light_pass.setUniform("light_position", light.position);
            shader_light_pass.setUniform("light_color", (Eigen::Vector3f)(light.color * light.brightness));
            glDrawElements(GL_TRIANGLES, unit_sphere->indices_size, GL_UNSIGNED_INT, nullptr);
        }

        //Copy depth data so the framebuffer has proper depth data for compositing
        glBindFramebuffer(GL_READ_FRAMEBUFFER, g_buffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, getMainFramebuffer().frame_buffer);
        glBlitFramebuffer(
                0, 0, getMainFramebuffer().width, getMainFramebuffer().height, 0, 0, getMainFramebuffer().width, getMainFramebuffer().height, GL_DEPTH_BUFFER_BIT, GL_NEAREST
        );

        //clean up
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

    DeferredPipelineOgl::DeferredPipelineOgl(int width, int height, const Camera &camera) : camera(camera) , PipelineOgl(width,height) {
        glGenVertexArrays(1, &empty_vao);
        unit_sphere = loadMeshOgl(loadOBJ("./meshes/unit_sphere.obj")[0]);
    }
} // EngiGraph