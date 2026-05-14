#include <LE/Physics/PhysicsTests.hpp>

namespace le
{
    std::optional<RayAabbIntersection> PhysicsTests::Intersect(const Ray& ray, const AABB& aabb)
    {
        float tmin = 0.0f;
        float tmax = std::numeric_limits<float>::max();

        for (size_t i = 0; i < 3; i++)
        {
            float t1 = (aabb.min[i] - ray.org[i]) * ray.inv[i];
            float t2 = (aabb.max[i] - ray.org[i]) * ray.inv[i];
            tmin = std::max(tmin, std::min(t1, t2));
            tmax = std::min(tmax, std::max(t1, t2));
        }

        if (tmax >= tmin)
            return RayAabbIntersection{tmin, tmax};

        return std::nullopt;
    }

    bool PhysicsTests::IntersectsX(const AABB& a, const AABB& b)
    {
        return a.min.x < b.max.x && a.max.x > b.min.x;
    }

    bool PhysicsTests::IntersectsY(const AABB& a, const AABB& b)
    {
        return a.min.y < b.max.y && a.max.y > b.min.y;
    }

    bool PhysicsTests::IntersectsZ(const AABB& a, const AABB& b)
    {
        return a.min.z < b.max.z && a.max.z > b.min.z;
    }

    bool PhysicsTests::Intersects(const AABB& a, const AABB& b)
    {
        return IntersectsX(a, b) && IntersectsY(a, b) && IntersectsZ(a, b);
    }
}
