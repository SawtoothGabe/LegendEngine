#pragma once

#include <LE/Math/Types.hpp>

namespace le
{
    struct AABB
    {
        void Expand(Vector3f value);
        void Grow(Vector3f value);
        void Move(Vector3f value);

        Vector3f min;
        Vector3f max;
    };
}
