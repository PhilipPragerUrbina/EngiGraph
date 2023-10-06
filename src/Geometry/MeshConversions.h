//
// Created by Philip on 10/4/2023.
//

#pragma once
#include "Mesh.h"
#include "VisualMesh.h"
namespace EngiGraph {

    /**
     * Removes visual data(normals and texture coordinates) and reduces visual mesh to valid base mesh.
     * @param input_mesh Input mesh.
     * @return Geometry only mesh with unique vertices.
     */
    Mesh stripVisualMesh(const VisualMesh& input_mesh);

} // EngiGraph
