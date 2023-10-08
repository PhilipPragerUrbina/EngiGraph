//
// Created by Philip on 9/20/2023.
//

#pragma once
#include "AngleDegrees.h"
#include "AngleRadians.h"

namespace EngiGraph {

    /**
     * Convert radians to degrees.
     * @details See Constants.h to see internal PI value.
     * @tparam T Internal type
     * @param angle Input angle in radians.
     * @return Output angle in degrees.
     */
    template <typename  T> AngleDegrees<T> convert(AngleRadians<T> angle);

    /**
     * Convert degrees to radians.
     * @details See Constants.h to see internal PI value.
     * @tparam T Internal type
     * @param angle Input angle in degrees.
     * @return Output angle in radians.
     */
    template <typename  T> AngleRadians<T> convert(AngleDegrees<T> angle);

} // EngiGraph
