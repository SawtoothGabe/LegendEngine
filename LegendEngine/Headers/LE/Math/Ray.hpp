#pragma once

#include <LE/Math/Types.hpp>

namespace le
{
    struct RayAabbIntersection
    {
        // The start of the intersection expressed by a multiple of the ray direction
        float tmin = 0.0f;
        // The end of the intersection expressed by a multiple of the ray direction
        float tmax = 0.0f;
    };

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
