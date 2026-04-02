#pragma once

namespace le
{
    struct Component
    {
        virtual ~Component() = default;
        bool enabled = true;
    };
}