//
// Created by Philip on 9/20/2023.
//

#pragma once

namespace EngiGraph {

    /**
     * Degree angle unit.
     * @tparam T Internal type
     */
    template <typename T> class AngleDegrees {
    public:
        explicit AngleDegrees(T value) : value(value){}

        T value;
    };

} // EngiGraph
