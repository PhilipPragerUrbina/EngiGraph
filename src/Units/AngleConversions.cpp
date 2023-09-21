//
// Created by Philip on 9/20/2023.
//

#include "AngleConversions.h"
#include "Constants.h"

namespace EngiGraph {

    template<typename T>
    AngleDegrees<T> convert(AngleRadians<T> angle) {
        return AngleDegrees<T>(angle.value * ((T)180.0/(T)CONSTANT_PI));
    }

    template<typename T>
    AngleRadians<T> convert(AngleDegrees<T> angle) {
        return AngleRadians<T>(angle.value * ((T)CONSTANT_PI/(T)180.0));
    }

    template AngleRadians<float> convert(AngleDegrees<float> angle);
    template AngleDegrees<float> convert(AngleRadians<float> angle);

    template AngleRadians<double> convert(AngleDegrees<double> angle);
    template AngleDegrees<double> convert(AngleRadians<double> angle);


} // EngiGraph