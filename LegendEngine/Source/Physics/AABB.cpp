#include <LE/Physics/AABB.hpp>
#include <LE/Physics/PhysicsTests.hpp>

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

    AABB AABB::Expanded(const Vector3f value) const
    {
        AABB result = *this;
        result.Expand(value);
        return result;
    }

    AABB AABB::Grown(const Vector3f value) const
    {
        AABB result = *this;
        result.Grow(value);
        return result;
    }

    AABB AABB::Moved(const Vector3f value) const
    {
        AABB result = *this;
        result.Move(value);
        return result;
    }

    float AABB::GetClipX(const AABB& other, float delta) const
    {
        if (!PhysicsTests::IntersectsY(*this, other) || !PhysicsTests::IntersectsZ(*this, other))
            return delta;

        if (delta > 0.0f && max.x <= other.min.x)
        {
            const float clip = other.min.x - max.x;
            if (delta > clip)
                delta = clip;
        }

        if (delta < 0.0f && min.x >= other.max.x)
        {
            const float clip = other.max.x - min.x;
            if (delta < clip)
                delta = clip;
        }

        return delta;
    }

    float AABB::GetClipY(const AABB& other, float delta) const
    {
        if (!PhysicsTests::IntersectsX(*this, other) || !PhysicsTests::IntersectsZ(*this, other))
            return delta;

        if (delta > 0.0f && max.y <= other.min.y)
        {
            const float clip = other.min.y - max.y;
            if (delta > clip)
                delta = clip;
        }

        if (delta < 0.0f && min.y >= other.max.y)
        {
            const float clip = other.max.y - min.y;
            if (delta < clip)
                delta = clip;
        }

        return delta;
    }

    float AABB::GetClipZ(const AABB& other, float delta) const
    {
        if (!PhysicsTests::IntersectsX(*this, other) || !PhysicsTests::IntersectsY(*this, other))
            return delta;

        if (delta > 0.0f && max.z <= other.min.z)
        {
            const float clip = other.min.z - max.z;
            if (delta > clip)
                delta = clip;
        }

        if (delta < 0.0f && min.z >= other.max.z)
        {
            const float clip = other.max.z - min.z;
            if (delta < clip)
                delta = clip;
        }

        return delta;
    }

    Vector3f AABB::GetClip(const AABB& other, const Vector3f delta) const
    {
        return {
            GetClipX(other, delta.x),
            GetClipY(other, delta.y),
            GetClipZ(other, delta.z)
        };
    }
}
