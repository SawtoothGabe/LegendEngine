#pragma once

#include <LE/Math/Types.hpp>

namespace le
{
    struct AABB
    {
        void Expand(Vector3f value);
        void Grow(Vector3f value);
        void Move(Vector3f value);

        [[nodiscard]] AABB Expanded(Vector3f value) const;
        [[nodiscard]] AABB Grown(Vector3f value) const;
        [[nodiscard]] AABB Moved(Vector3f value) const;

        [[nodiscard]] float GetClipX(const AABB& other, float delta) const;
        [[nodiscard]] float GetClipY(const AABB& other, float delta) const;
        [[nodiscard]] float GetClipZ(const AABB& other, float delta) const;
        [[nodiscard]] Vector3f GetClip(const AABB& other, Vector3f delta) const;

        Vector3f min;
        Vector3f max;
    };
}
