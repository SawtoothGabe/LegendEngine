#pragma once

#include <LE/Components/Component.hpp>

namespace le
{
    struct ActiveCamera : Component
    {
        float priority = 1.0f;
    };
}
