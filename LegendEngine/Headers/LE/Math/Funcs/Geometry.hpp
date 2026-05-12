#pragma once

#include <optional>
#include <LE/Math/AABB.hpp>
#include <LE/Math/Ray.hpp>

namespace le::Math
{
    /**
     * Tests for the intersection between a ray and an axis-aligned bounding box (AABB).
     *
     * @param ray The ray to intersect with the AABB
     * @param aabb The AABB to intersect with the ray
     * @return An optional that contains the intersection data if there is an intersection, an empty optional otherwise.
     */
    std::optional<RayAabbIntersection> Intersect(const Ray& ray, const AABB& aabb);
}
