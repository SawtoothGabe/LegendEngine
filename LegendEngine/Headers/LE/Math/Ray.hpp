#pragma once

#include <LE/Math/Types.hpp>

namespace le
{
    struct Ray
    {
        Ray() = default;
        Ray(const Vector3f& origin, const Vector3f& direction);

        Vector3f org; // Origin
        Vector3f dir; // Direction
        Vector3f inv; // Multiplicative inverse of the direction
    private:
        void CalculateInverse();
    };
}
