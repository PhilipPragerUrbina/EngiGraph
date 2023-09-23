//
// Created by Philip on 9/23/2023.
//

#pragma once
#include "../ShaderOgl.h"
#include "../PipelineOgl.h"
#include "../../Camera.h"

namespace EngiGraph {

    struct SurfaceNormalPipeLineDrawInfo{
        Eigen::Matrix4f object_transform;
    };

    /**
     * Simple pipeline to visualize local vertex normals.
     */
    class SurfaceNormalPipeLineOgl : public PipelineOGL<SurfaceNormalPipeLineDrawInfo> {
    private:
        ShaderOGL shader{"shaders/normal_vertex.glsl", "shaders/normal_fragment.glsl"};
    protected:
        void resizeCallBack(int width, int height) override;
    public:
        Camera main_camera;
        explicit SurfaceNormalPipeLineOgl(const Camera& main_camera) : main_camera(main_camera){}

        void render(const EngiGraph::ResourcePoolOGL &resource_pool) override;

    };

} // EngiGraph
