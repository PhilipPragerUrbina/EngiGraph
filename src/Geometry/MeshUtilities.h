//
// Created by Philip on 10/4/2023.
//

#pragma once

#include "Mesh.h"

namespace EngiGraph {

    /**
     * Take in an invalid mesh with non-unique vertices, and make the vertices be unique.
     * @param input Input mesh.
     * @param combine_delta Maximum distance from which vertices are considered to be equal.
     * @return Mesh with unique vertices.
     */
    Mesh reduceMesh(const Mesh& input, float combine_delta = 0.001f);

} // EngiGraph
