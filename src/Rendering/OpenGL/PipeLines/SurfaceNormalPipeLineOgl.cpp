//
// Created by Philip on 9/23/2023.
//

#include "SurfaceNormalPipeLineOgl.h"

namespace EngiGraph {
    void SurfaceNormalPipeLineOgl::render(const ResourcePoolOGL &resource_pool) {
        glBindFramebuffer(GL_FRAMEBUFFER, getMainFramebuffer().frame_buffer);
        glViewport(0,0,getMainFramebuffer().width,getMainFramebuffer().height);
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.5f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        shader.setUniform("view",main_camera.getViewMatrix());
        shader.setUniform("projection",main_camera.getProjectionMatrix());

        std::vector<DrawCall> calls = popAllDrawCalls();

        for (const auto& call : calls) {
            shader.setUniform("model", call.attributes.object_transform);
            ResourcePoolOGL::OGLMesh mesh = resource_pool.getMesh(call.mesh);
            glBindVertexArray(mesh.vertex_array_id);
            glDrawElements(GL_TRIANGLES, mesh.indices_size, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);//todo move
    }

    void SurfaceNormalPipeLineOgl::resizeCallBack(int width, int height) {
        main_camera.setAspectRatio((float)width/(float)height);
    }
} // EngiGraph