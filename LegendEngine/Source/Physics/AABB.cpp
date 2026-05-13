#include <LE/Physics/AABB.hpp>

namespace le
{
    void AABB::Expand(const Vector3f value)
    {
        if (value.x > 0.0f)
            max.x += value.x;
        else
            min.x += value.x;

        if (value.y > 0.0f)
            max.y += value.y;
        else
            min.y += value.y;

        if (value.z > 0.0f)
            max.z += value.z;
        else
            min.z += value.z;
    }

    void AABB::Grow(const Vector3f value)
    {
        min -= value;
        max += value;
    }

    void AABB::Move(const Vector3f value)
    {
        min += value;
        max += value;
    }
}
