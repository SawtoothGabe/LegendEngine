#pragma once

#include <LE/Math/Math.hpp>

#include <LE/Components/Component.hpp>

namespace le
{
    struct Light : Component
    {
        enum class LightType
        {
            POINT = 0,
            SPOT = 1,
            SUN = 2,
        };

        Color color = Color(1.0f);
        LightType type = LightType::POINT;
    };
}
