//
// Created by Philip on 9/23/2023.
//

#include "SurfaceNormalPipeLineOgl.h"

namespace EngiGraph {
    void SurfaceNormalPipeLineOgl::render() {
        glBindFramebuffer(GL_FRAMEBUFFER, getMainFramebuffer().frame_buffer);
        glViewport(0,0,getMainFramebuffer().width,getMainFramebuffer().height);
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.5f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        shader.setUniform("view",main_camera.getViewMatrix());
        shader.setUniform("projection",main_camera.getProjectionMatrix());

        for (const auto& call : draw_calls) {
            shader.setUniform("model", call.transform);
            glBindVertexArray(call.mesh->vertex_array_id);
            glDrawElements(GL_TRIANGLES, call.mesh->indices_size, GL_UNSIGNED_INT, nullptr);

        }
        glBindVertexArray(0);
        draw_calls.clear();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void SurfaceNormalPipeLineOgl::resizeCallBack(int width, int height) {
        main_camera.setAspectRatio((float)width/(float)height);
    }

    SurfaceNormalPipeLineOgl::SurfaceNormalPipeLineOgl(const Camera &main_camera, int width, int height) : main_camera(main_camera) , PipelineOgl(width,height) {}

    void SurfaceNormalPipeLineOgl::submitDrawCall(const std::shared_ptr<MeshResourceOgl> &mesh,
                                                  const Eigen::Matrix4f &transform) {
        draw_calls.push_back({mesh,transform});
    }
} // EngiGraph