//
// Created by Philip on 9/20/2023.
//

#pragma once

namespace EngiGraph {

    /**
     * Radian angle unit.
     * @tparam T Internal type
     */
    template <typename T> class AngleRadians {
    public:
        explicit AngleRadians(T value) : value(value){}

        T value;
    };

} // EngiGraph
