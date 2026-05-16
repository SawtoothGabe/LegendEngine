#pragma once

#include <optional>

#include <LE/Math/Ray.hpp>
#include <LE/Physics/AABB.hpp>

namespace le
{
    struct RayAabbIntersection
    {
        // The start of the intersection expressed by a multiple of the ray direction
        float tmin = 0.0f;
        // The end of the intersection expressed by a multiple of the ray direction
        float tmax = 0.0f;
    };
}

namespace le::PhysicsTests
{
    /**
     * Tests for the intersection between a ray and an axis-aligned bounding box (AABB).
     *
     * @param ray The ray to intersect with the AABB
     * @param aabb The AABB to intersect with the ray
     * @return An optional that contains the intersection data if there is an intersection, an empty optional otherwise.
     */
    std::optional<RayAabbIntersection> Intersects(const Ray& ray, const AABB& aabb);

    bool IntersectsX(const AABB& a, const AABB& b);
    bool IntersectsY(const AABB& a, const AABB& b);
    bool IntersectsZ(const AABB& a, const AABB& b);
    bool Intersects(const AABB& a, const AABB& b);
}