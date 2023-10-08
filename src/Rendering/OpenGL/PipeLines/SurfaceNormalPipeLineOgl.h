//
// Created by Philip on 9/23/2023.
//

#pragma once
#include "./src/Rendering/OpenGL/ShaderOgl.h"
#include "./src/Rendering/OpenGL/PipelineOgl.h"
#include "./src/Rendering/Camera.h"

namespace EngiGraph {

    /**
     * Simple pipeline to visualize local vertex normals.
     */
    class SurfaceNormalPipeLineOgl : public PipelineOgl {
    private:
        ShaderOGL shader{"shaders/normal_vertex.glsl", "shaders/normal_fragment.glsl"};

        struct DrawCall{
            std::shared_ptr<MeshResourceOgl> mesh;
            Eigen::Matrix4f transform;
        };

        std::vector<DrawCall> draw_calls{};
    protected:
        void resizeCallBack(int width, int height) override;
    public:

        /**
         * Submit a mesh to be rendered.
         * @param mesh Mesh to render(Expects vertex,normal,uv) attributes.
         * @param transform The object transform of the mesh.
         */
        void submitDrawCall(const std::shared_ptr<MeshResourceOgl>& mesh, const Eigen::Matrix4f& transform);

        Camera main_camera;

        explicit SurfaceNormalPipeLineOgl(const Camera& main_camera, int width, int height);

        void render() override;

    };

} // EngiGraph
