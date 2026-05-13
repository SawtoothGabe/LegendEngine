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
}
