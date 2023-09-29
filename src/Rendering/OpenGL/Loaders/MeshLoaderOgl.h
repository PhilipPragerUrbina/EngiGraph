//
// Created by Philip on 9/28/2023.
//

#pragma once
#include <memory>
#include "../Resources/MeshResourceOgl.h"
#include "../../../Geometry/VisualMesh.h"

namespace EngiGraph {

    /**
     * Load an indexed triangular visual mesh from the cpu to the GPU for openGL to use.
     * @param cpu_mesh Mesh data on cpu.
     * @throws RuntimeException Issue loading mesh.
     * @return A pointer to a gpu resource that will be deallocated when the pointer is destroyed.
     */
    std::shared_ptr<MeshResourceOgl> loadMesh(const VisualMesh& cpu_mesh);

} // EngiGraph
