//
// Created by Philip on 9/18/2023.
//

#pragma once

#include "../Geometry/VisualMesh.h"
#include <vector>

namespace EngiGraph {

    /**
     * Load geometry from an OBJ file.
     * @param filename Location of OBJ file.
     * @throws RuntimeException Problem loading file.
     * @details Will triangulate shapes. This will also auto generate normals and texture coordinates if they are missing.
     * @return Geometry of each OBJ shape. Can be combined into a single mesh later if desired.
     */
    std::vector<VisualMesh> loadOBJ(const std::string& filename);

} // EngiGraph
